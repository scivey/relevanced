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
    _handleResponseStatus: function(response) {
        // duck typing has started to disgust me
        var status;
        if (_.has(response, 'status')) {
            status = response.status;
        } else if (_.has(response, 'code')) {
            status = response;
        } else {
            return "";
        }
        if (status.code === StatusCode.OK) {
            return "";
        }
        errorMap[StatusCode.DOCUMENT_DOES_NOT_EXIST] = "Document does not exist";
        errorMap[StatusCode.CENTROID_DOES_NOT_EXIST] = "Centroid does not exist";
        errorMap[StatusCode.DOCUMENT_ALREADY_EXISTS] = "Document already exists";
        errorMap[StatusCode.CENTROID_ALREADY_EXISTS] = "Centroid already exists";
        errorMap[StatusCode.UNKNOWN_EXCEPTION] = "Unknown exception.";
        if (_.has(errorMap, status.code)) {
            return errorMap[status.code];
        }
        return "Unknown exception";
    },
    getDocumentSimilarity: function(centroidId, docId) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.getDocumentSimilarity(centroidId, docId, function(err, res) {
                if (err) return reject(err);
                var errMsg = self._handleResponseStatus(res);
                if (errMsg) {
                    return reject(new Error(errMsg));
                }
                return resolve(res.similarity);
            });
        });
    },
    getTextSimilarity: function(centroidId, text) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.getTextSimilarity(centroidId, text, function(err, res) {
                if (err) return reject(err);
                var errMsg = self._handleResponseStatus(res);
                if (errMsg) {
                    return reject(new Error(errMsg));
                }
                return resolve(res.similarity);
            });
        });
    },
    multiGetTextSimilarity: function(centroidIdList, text) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.multiGetTextSimilarity(centroidIdList, text, function(err, res) {
                if (err) return reject(err);
                var errMsg = self._handleResponseStatus(res);
                if (errMsg) {
                    return reject(new Error(errMsg));
                }
                return resolve(res.scores);
            });
        });
    },
    createDocument: function(text) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.createDocument(text, function(err, res) {
                if (err) return reject(err);
                var errMsg = self._handleResponseStatus(res);
                if (errMsg) {
                    return reject(new Error(errMsg));
                }
                return resolve(res.created);
            });
        });
    },
    createDocumentWithID: function(id, text) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.createDocumentWithID(id, text, function(err, res) {
                if (err) return reject(err);
                var errMsg = self._handleResponseStatus(res);
                if (errMsg) {
                    return reject(new Error(errMsg));
                }
                return resolve(res.created);
            });
        });
    },
    deleteDocument: function(id) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.deleteDocument(id, function(err, res) {
                if (err) return reject(err);
                var errMsg = self._handleResponseStatus(res);
                if (errMsg) {
                    return reject(new Error(errMsg));
                }
                return resolve(res.created);
            });
        });
    },
    createCentroid: function(id) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.createCentroid(id, function(err, res) {
                if (err) return reject(err);
                var errMsg = self._handleResponseStatus(res);
                if (errMsg) {
                    return reject(new Error(errMsg));
                }
                return resolve(res.created);
            });
        });
    },
    deleteCentroid: function(id) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.deleteCentroid(id, function(err, res) {
                if (err) return reject(err);
                var errMsg = self._handleResponseStatus(res);
                if (errMsg) {
                    return reject(new Error(errMsg));
                }
                return resolve(res.created);
            });
        });
    },
    addDocumentToCentroid: function(centroidId, documentId) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.addDocumentToCentroid(centroidId, documentId, function(err, res) {
                if (err) return reject(err);
                var errMsg = self._handleResponseStatus(res);
                if (errMsg) {
                    return reject(new Error(errMsg));
                }
                return resolve(true);
            });
        });
    },
    removeDocumentFromCentroid: function(centroidId, documentId) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.removeDocumentFromCentroid(centroidId, documentId, function(err, res) {
                if (err) return reject(err);
                var errMsg = self._handleResponseStatus(res);
                if (errMsg) {
                    return reject(new Error(errMsg));
                }
                return resolve(true);
            });
        });
    },
    recomputeCentroid: function(centroidId) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.recomputeCentroid(centroidId, function(err, res) {
                if (err) return reject(err);
                var errMsg = self._handleResponseStatus(res);
                if (errMsg) {
                    return reject(new Error(errMsg));
                }
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
    }
});

module.exports = {
    getRawClient: getRawClient,
    RelevancedClient: RelevancedClient
};

