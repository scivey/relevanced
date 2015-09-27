var Promise = require('bluebird');
var _ = require('lodash');
var thrift = require('thrift');
var thriftTransports = require('thrift/lib/thrift/transport');
var thriftProtocols = require('thrift/lib/thrift/protocol');
var generated = require('./gen-nodejs/Relevance');
var ttypes = require('./gen-nodejs/TextRelevance_types');
var RelevanceStatus = ttypes.RelevanceStatus;

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
    getRelevanceForDoc: function(classifierId, docId) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.getRelevanceForDoc(classifierId, docId, function(err, res) {
                if (err) return reject(err);
                if (res.status !== RelevanceStatus.OK) {
                    if (res.status === RelevanceStatus.CLASSIFIER_DOES_NOT_EXIST) {
                        return reject(
                            new Error("Classifier does not exist: " + classifierId);
                        );
                    } else if (res.status == RelevanceStatus.DOCUMENT_DOES_NOT_EXIST) {
                        return reject(
                            new Error("Document does not exist: " + docId)
                        );
                    }
                    return reject(
                        new Error("Unknown exception.")
                    );
                }
                return resolve(res.relevance);
            });
        });
    },
    getRelevanceForText: function(classifierId, text) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.getRelevanceForText(classifierId, text, function(err, res) {
                if (err) return reject(err);
                if (res.status !== RelevanceStatus.OK) {
                    if (res.status === RelevanceStatus.CLASSIFIER_DOES_NOT_EXIST) {
                        return reject(
                            new Error("Classifier does not exist: " + classifierId);
                        );
                    }
                    return reject(
                        new Error("Unknown exception.")
                    );
                }
                return resolve(res.relevance);
            });
        });
    },
    createDocument: function(text) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.createDocument(text, function(err, res) {
                if (err) return reject(err);
                if (res.status !== RelevanceStatus.OK) {
                    return reject(
                        new Error("Unknown exception.")
                    );
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
                if (res.status !== RelevanceStatus.OK) {
                    if (res.status === RelevanceStatus.DOCUMENT_ALREADY_EXISTS) {
                        return reject(
                            new Error("Document already exists: " + id);
                        )
                    }
                    return reject(
                        new Error("Unknown exception.")
                    );
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
                if (res.status !== RelevanceStatus.OK) {
                    if (res.status === RelevanceStatus.DOCUMENT_DOES_NOT_EXIST) {
                        return reject(
                            new Error("Document does not exist: " + id);
                        )
                    }
                    return reject(
                        new Error("Unknown exception.")
                    );
                }
                return resolve(res.created);
            });
        });
    },
    createClassifier: function(id) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.createClassifier(id, function(err, res) {
                if (err) return reject(err);
                if (res.status !== RelevanceStatus.OK) {
                    if (res.status === RelevanceStatus.CLASSIFIER_ALREADY_EXISTS) {
                        return reject(
                            new Error("Classifier already exists: " + id)
                        );
                    }
                    return reject(
                        new Error("Unknown exception.")
                    );
                }
                return resolve(res.created);
            });
        });
    },
    deleteClassifier: function(id) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.deleteClassifier(id, function(err, res) {
                if (err) return reject(err);
                if (res.status !== RelevanceStatus.OK) {
                    if (res.status === RelevanceStatus.CLASSIFIER_DOES_NOT_EXIST) {
                        return reject(
                            new Error("Classifier does not exist: " + id)
                        );
                    }
                    return reject(
                        new Error("Unknown exception.")
                    );
                }
                return resolve(res.created);
            });
        });
    },
    addPositiveDocumentToClassifier: function(classifierId, documentId) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.addPositiveDocumentToClassifier(classifierId, documentId, function(err, res) {
                if (err) return reject(err);
                if (res.status !== RelevanceStatus.OK) {
                    if (res.status === RelevanceStatus.CLASSIFIER_DOES_NOT_EXIST) {
                        return reject(
                            new Error("Classifier does not exist: " + classifierId);
                        );
                    } else if (res.status === RelevanceStatus.DOCUMENT_DOES_NOT_EXIST) {
                        return reject(
                            new Error("Document does not exist: " + documentId)
                        );
                    }
                    return reject(
                        new Error("Unknown exception.")
                    );
                }
                return resolve(res.created);
            });
        });
    },
    addNegativeDocumentToClassifier: function(classifierId, documentId) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.addNegativeDocumentToClassifier(classifierId, documentId, function(err, res) {
                if (err) return reject(err);
                if (res.status !== RelevanceStatus.OK) {
                    if (res.status === RelevanceStatus.CLASSIFIER_DOES_NOT_EXIST) {
                        return reject(
                            new Error("Classifier does not exist: " + classifierId);
                        );
                    } else if (res.status === RelevanceStatus.DOCUMENT_DOES_NOT_EXIST) {
                        return reject(
                            new Error("Document does not exist: " + documentId)
                        );
                    }
                    return reject(
                        new Error("Unknown exception.")
                    );
                }
                return resolve(res.created);
            });
        });
    },
    removeDocumentFromClassifier: function(classifierId, documentId) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.removeDocumentFromClassifier(classifierId, documentId, function(err, res) {
                if (err) return reject(err);
                if (res.status !== RelevanceStatus.OK) {
                    if (res.status === RelevanceStatus.CLASSIFIER_DOES_NOT_EXIST) {
                        return reject(
                            new Error("Classifier does not exist: " + classifierId);
                        );
                    } else if (res.status === RelevanceStatus.DOCUMENT_DOES_NOT_EXIST) {
                        return reject(
                            new Error("Document does not exist: " + documentId)
                        );
                    }
                    return reject(
                        new Error("Unknown exception.")
                    );
                }
                return resolve(res.created);
            });
        });
    },
    recompute: function(classifierId) {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.recompute(classifierId, function(err, res) {
                if (err) return reject(err);
                if (res.status !== RelevanceStatus.OK) {
                    if (res.status === RelevanceStatus.CLASSIFIER_DOES_NOT_EXIST) {
                        return reject(
                            new Error("Classifier does not exist: " + classifierId);
                        );
                    }
                    return reject(
                        new Error("Unknown exception.")
                    );
                }
                return resolve();
            });
        });
    },
    listClassifiers: function() {
        var self = this;
        return new Promise(function(resolve, reject) {
            return self.client.listClassifiers(function(err, res) {
                if (err) return reject(err);
                return resolve(res);
            })
        });
    },
    listDocuments: function() {
        var self = this;
        return new Promise(function(resolve, reject) {
            self.client.listDocuments(function(err, res) {
                if (err) return reject(err);
                return resolve(res);
            });
        });
    },
    getClassifierSize: function(classifierId) {
        return new Promise(function(resolve, reject) {
            return self.client.getClassifierSize(classifierId, function(err, res) {
                if (err) return reject(err);
                if (res.status !== RelevanceStatus.OK) {
                    if (res.status === RelevanceStatus.CLASSIFIER_DOES_NOT_EXIST) {
                        return reject(
                            new Error("Classifier does not exist: " + classifierId);
                        );
                    }
                    return reject(
                        new Error("Unknown exception.")
                    );
                }
                return resolve(res.size);
            });
        });
    }
});

module.exports = {
    getRawClient: getRawClient,
    RelevancedClient: RelevancedClient
};

