from func_test_lib.multiclass import load, test, recompute
import sys

if __name__ == '__main__':
    funcs = {
        'load': load,
        'test': test,
        'recompute': recompute
    }
    funcs[sys.argv[1]]()
