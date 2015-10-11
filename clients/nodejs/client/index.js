var Promise = require('bluebird');
var _ = require('lodash');
var thrift = require('thrift');
var thriftTransports = require('thrift/lib/thrift/transport');
var thriftProtocols = require('thrift/lib/thrift/protocol');
var generated = require('./gen-nodejs/Relevanced');

var RelevancedClient = function(hostname, portno) {
    this.hostname = hostname;
    this.portno = portno;
};

_.extend(RelevancedClient.prototype, {
    connect: function() {
        var self = this;
        return new Promise(function(resolve, reject) {
            try {
                self.conn = thrift.createConnection(self.hostname, self.portno, {
                    transport: thriftTransports.TBufferedTransport(),
                    protocol: thriftProtocols.TBinaryProtocol()
                });
                self.client = thrift.createClient(generated.Client, self.conn);
                return resolve(self);
            } catch (e) {
                return reject(e);
            }
        });
    },
    disconnect: function() {
        var self = this;
        return new Promise(function(resolve, reject) {
            try {
                self.conn.end();
                return resolve();
            } catch (e) {
                return reject(e);
            }
        });
    }
});

RelevancedClient.connect = function(hostname, portno) {
    var client = new RelevancedClient(hostname, portno);
    return client.connect();
};

var CLIENT_METHODS = [
    'ping',
    'getServerMetadata',
    'getDocumentSimilarity',
    'multiGetTextSimilarity',
    'getTextSimilarity',
    'getCentroidSimilarity',
    'createDocument',
    'createDocumentWithID',
    'deleteDocument',
    'getDocumentMetadata',
    'createCentroid',
    'listAllDocumentsForCentroid',
    'addDocumentToCentroid',
    'removeDocumentFromCentroid',
    'joinCentroid',
    'listAllCentroids',
    'listAllDocuments'
];

_.each(CLIENT_METHODS, function(methodName) {
    RelevancedClient.prototype[methodName] = function() {
        var args = _.toArray(arguments);
        var self = this;
        return new Promise(function(resolve, reject) {
            if (!_.has(self, 'client')) {
                return reject(new Error("Not connected!"));
            }
            args.push(function(err, result) {
                if (err) return reject(err);
                return resolve(result);
            });
            try {
                self.client[methodName].apply(self.client, args);
            } catch (e) {
                return reject(e);
            }
        });
    };
});

module.exports = {
    RelevancedClient: RelevancedClient
};

