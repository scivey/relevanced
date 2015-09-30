from __future__ import print_function
from functools import wraps
from pprint import pprint
import base64
import md5
import pickle
import memcache

def memoize0(func):
    _closure = {}
    @wraps(func)
    def out():
        if 'result' not in _closure:
            _closure['result'] = func()
        return _closure['result']
    return out

def memoize1(func):
    _closure = {}
    @wraps(func)
    def out(x):
        if x not in _closure:
            _closure[x] = func(x)
        return _closure[x]
    return out

class CacheMissException(Exception):
    pass

class CacheClient(object):
    def __init__(self, prefix, ttl):
        self.prefix = prefix
        self.ttl = int(ttl)

    @property
    def client(self):
        if not hasattr(self, '_client'):
            self._client = memcache.Client(['127.0.0.1:11211'])
        return self._client

    def calc_key(self, key):
        key = "%s:%s" % (self.prefix, key)
        if len(key) > 200:
            digest = md5.md5(key).hexdigest()
            key = key[:len(digest)] + digest
        return key

    def get(self, key):
        res = self.client.get(self.calc_key(key))
        if res == None:
            raise CacheMissException("Cache miss: %s" % key)
        return self.val_from_mc(res)

    def get_multi(self, keys):
        to_calced = {}
        from_calced = {}
        for key in keys:
            calced = self.calc_key(key)
            from_calced[calced] = key
            to_calced[key] = calced

        fetched = self.client.get_multi(to_calced.values())
        output = {}
        missing = []
        for k, v in fetched.iteritems():
            output[from_calced[k]] = self.val_from_mc(v)
        for key in keys:
            if key not in output:
                missing.append(key)
        return output, missing

    def val_from_mc(self, val):
        return pickle.loads(base64.b64decode(val))

    def val_to_mc(self, val):
        return base64.b64encode(pickle.dumps(val))

    def set(self, key, val):
        return self.client.set(self.calc_key(key), self.val_to_mc(val))

    def set_multi(self, val_dict):
        mc_dict = {}
        for k, v in val_dict.iteritems():
            mc_dict[self.calc_key(k)] = self.val_to_mc(v)
        return self.client.set_multi(mc_dict)


class FuncCache(object):
    def __init__(self, func, prefix, ttl):
        self.func = func
        self.prefix = prefix
        self.ttl = int(ttl)

    @property
    def client(self):
        if not hasattr(self, '_client'):
            self._client = CacheClient(prefix=self.prefix, ttl=self.ttl)
        return self._client

    def get(self, key):
        try:
            result = self.client.get(key)
        except CacheMissException:
            result = self.func(key)
            self.client.set(key, result)
        return result

    def get_multi(self, keys):
        found, missing = self.client.get_multi(keys)
        if missing:
            to_set = {}
            for missing_key in missing:
                result = self.func(missing_key)
                to_set[missing_key] = result
                found[missing_key] = result
            self.client.set_multi(to_set)
        return found

