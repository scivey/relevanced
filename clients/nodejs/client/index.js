var Promise = require('bluebird');
var _ = require('lodash');
var thrift = require('thrift');
var thriftTransports = require('thrift/lib/thrift/transport');
var thriftProtocols = require('thrift/lib/thrift/protocol');
var generated = require('./gen-nodejs/Relevanced');
var ttypes = require('./gen-nodejs/RelevancedProtocol_types');
var Status = ttypes.Status;
var StatusCode = ttypes.StatusCode;

var getRawClient = function(hostname, portno) {
    var conn = thrift.createConnection(hostname, portno, {
        transport: thriftTransports.TBufferedTransport(),
        protocol: thriftProtocols.TBinaryProtocol()
    });
    return thrift.createClient(generated.Client, conn);
};

var RelevancedClient = function(hostname, portno) {
    this.hostname = hostname;
    this.portno = portno;
    this.client = getRawClient(hostname, portno);
};

_.extend(RelevancedClient.prototype, {
    getDocumentSimilarity: function(centroidId, docId) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.getDocumentSimilarity(centroidId, docId, function(err, res) {
                if (err) return reject(err);
                return resolve(res.similarity);
            });
        });
    },
    getTextSimilarity: function(centroidId, text) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.getTextSimilarity(centroidId, text, function(err, res) {
                if (err) return reject(err);
                return resolve(res.similarity);
            });
        });
    },
    multiGetTextSimilarity: function(centroidIdList, text) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.multiGetTextSimilarity(centroidIdList, text, function(err, res) {
                if (err) return reject(err);
                return resolve(res.scores);
            });
        });
    },
    createDocument: function(text) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.createDocument(text, function(err, res) {
                if (err) return reject(err);
                return resolve(res.id);
            });
        });
    },
    createDocumentWithID: function(id, text) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.createDocumentWithID(id, text, function(err, res) {
                if (err) return reject(err);
                return resolve(res.id);
            });
        });
    },
    deleteDocument: function(id) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.deleteDocument(id, function(err, res) {
                if (err) return reject(err);
                return resolve(res.id);
            });
        });
    },
    createCentroid: function(id) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.createCentroid(id, function(err, res) {
                if (err) return reject(err);
                return resolve(res.id);
            });
        });
    },
    deleteCentroid: function(id) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.deleteCentroid(id, function(err, res) {
                if (err) return reject(err);
                return resolve(res.id);
            });
        });
    },
    addDocumentToCentroid: function(centroidId, documentId) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.addDocumentToCentroid(centroidId, documentId, function(err, res) {
                if (err) return reject(err);
                return resolve(true);
            });
        });
    },
    removeDocumentFromCentroid: function(centroidId, documentId) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.removeDocumentFromCentroid(centroidId, documentId, function(err, res) {
                if (err) return reject(err);
                return resolve(true);
            });
        });
    },
    joinCentroid: function(centroidId) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.joinCentroid(centroidId, function(err, res) {
                if (err) return reject(err);
                return resolve(true);
            });
        });
    },
    listAllCentroids: function() {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.listAllCentroids(function(err, res) {
                if (err) return reject(err);
                return resolve(res);
            })
        });
    },
    listAllDocuments: function() {
        var self = this;
        return new Promise(function(resolve, reject) {
            self.client.listAllDocuments(function(err, res) {
                if (err) return reject(err);
                return resolve(res);
            });
        });
    },
    getServerMetadata: function() {
        var self = this;
        return new Promise(function(resolve, reject) {
            self.client.getServerMetadata(function(err, res) {
                if (err) return reject(err);
                return resolve(res);
            });
        });
    }
});

module.exports = {
    getRawClient: getRawClient,
    RelevancedClient: RelevancedClient
};

