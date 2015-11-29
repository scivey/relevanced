var Promise = require('bluebird');
var _ = require('lodash');
var thrift = require('thrift');
var thriftTransports = require('thrift/lib/thrift/transport');
var thriftProtocols = require('thrift/lib/thrift/protocol');
var generated = require('./gen-nodejs/Relevanced');
var genTypes = require('./gen-nodejs/RelevancedProtocol_types');

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
    'multiDeleteDocuments',
    'getDocumentMetadata',
    'deleteCentroid',
    'multiDeleteCentroids',
    'createCentroid',
    'multiCreateCentroids',
    'listAllDocumentsForCentroid',
    'addDocumentsToCentroid',
    'removeDocumentsFromCentroid',
    'joinCentroid',
    'multiJoinCentroids',
    'listAllCentroids',
    'listAllDocuments',
    'listUnusedDocuments'
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

(function() {
    var original = RelevancedClient.prototype.createDocument;
    RelevancedClient.prototype.createDocument = function(text, lang) {
        if (!_.isNumber(lang)) {
            lang = genTypes.Language.EN;
        }
        return original.apply(this, [text, lang]);
    };
})();

(function() {
    var original = RelevancedClient.prototype.createDocumentWithID;
    RelevancedClient.prototype.createDocumentWithID = function(id, text, lang) {
        if (!_.isNumber(lang)) {
            lang = genTypes.Language.EN;
        }
        return original.apply(this, [id, text, lang]);
    };
})();

(function() {
    var original = RelevancedClient.prototype.getTextSimilarity;
    RelevancedClient.prototype.getTextSimilarity = function(centroid, text, lang) {
        if (!_.isNumber(lang)) {
            lang = genTypes.Language.EN;
        }
        return original.apply(this, [centroid, text , lang]);
    };
})();

(function() {
    var original = RelevancedClient.prototype.multiGetTextSimilarity;
    RelevancedClient.prototype.multiGetTextSimilarity = function(centroids, text, lang) {
        if (!_.isNumber(lang)) {
            lang = genTypes.Language.EN;
        }
        return original.apply(this, [centroids, text , lang]);
    };
})();

(function() {
    var original = RelevancedClient.prototype.multiGetTextSimilarity;
    RelevancedClient.prototype.multiGetTextSimilarity = function(centroids, text, lang) {
        if (!_.isNumber(lang)) {
            lang = genTypes.Language.EN;
        }
        return original.apply(this, [centroids, text , lang]);
    };
})();

(function() {
    var original = RelevancedClient.prototype.createCentroid;
    RelevancedClient.prototype.createCentroid = function(id, ignoreExisting) {
        if (!_.isBoolean(ignoreExisting)) {
            ignoreExisting = false;
        }
        var request = new genTypes.CreateCentroidRequest;
        request.id = id;
        request.ignoreExisting = ignoreExisting;
        return original.apply(this, [request]);
    };
})();

(function() {
    var original = RelevancedClient.prototype.multiCreateCentroids;
    RelevancedClient.prototype.multiCreateCentroids = function(ids, ignoreExisting) {
        if (!_.isBoolean(ignoreExisting)) {
            ignoreExisting = false;
        }
        var request = new genTypes.MultiCreateCentroidsRequest;
        request.ids = ids;
        request.ignoreExisting = ignoreExisting;
        return original.apply(this, [request]);
    };
})();

(function() {
    var original = RelevancedClient.prototype.deleteCentroid;
    RelevancedClient.prototype.deleteCentroid = function(id, ignoreMissing) {
        if (!_.isBoolean(ignoreMissing)) {
            ignoreMissing = false;
        }
        var request = new genTypes.DeleteCentroidRequest;
        request.id = id;
        request.ignoreMissing = ignoreMissing;
        return original.apply(this, [request]);
    };
})();

(function() {
    var original = RelevancedClient.prototype.multiDeleteCentroids;
    RelevancedClient.prototype.multiDeleteCentroids = function(ids, ignoreMissing) {
        if (!_.isBoolean(ignoreMissing)) {
            ignoreMissing = false;
        }
        var request = new genTypes.MultiDeleteCentroidsRequest;
        request.ids = ids;
        request.ignoreMissing = ignoreMissing;
        return original.apply(this, [request]);
    };
})();

(function() {
    var original = RelevancedClient.prototype.addDocumentsToCentroid;
    RelevancedClient.prototype.addDocumentsToCentroid = function(centroidId, documentIds, ignoreAlreadyInCentroid) {
        if (!_.isBoolean(ignoreAlreadyInCentroid)) {
            ignoreAlreadyInCentroid = false;
        }
        var request = new genTypes.AddDocumentsToCentroidRequest;
        request.centroidId = centroidId;
        request.documentIds = documentIds;
        request.ignoreAlreadyInCentroid = ignoreAlreadyInCentroid;
        return original.apply(this, [request]);
    };
})();

(function() {
    var original = RelevancedClient.prototype.removeDocumentsFromCentroid;
    RelevancedClient.prototype.removeDocumentsFromCentroid = function(centroidId, documentIds, ignoreNotInCentroid) {
        if (!_.isBoolean(ignoreNotInCentroid)) {
            ignoreNotInCentroid = false;
        }
        var request = new genTypes.RemoveDocumentsFromCentroidRequest;
        request.centroidId = centroidId;
        request.documentIds = documentIds;
        request.ignoreNotInCentroid = ignoreNotInCentroid;
        return original.apply(this, [request]);
    };
})();

(function() {
    var original = RelevancedClient.prototype.deleteDocument;
    RelevancedClient.prototype.deleteDocument = function(documentId, ignoreMissing) {
        if (!_.isBoolean(ignoreMissing)) {
            ignoreMissing = false;
        }
        var request = new genTypes.DeleteDocumentRequest;
        request.id = documentId;
        request.ignoreMissing = ignoreMissing;
        return original.apply(this, [request]);
    };
})();

(function() {
    var original = RelevancedClient.prototype.multiDeleteDocuments;
    RelevancedClient.prototype.multiDeleteDocuments = function(documentIds, ignoreMissing) {
        if (!_.isBoolean(ignoreMissing)) {
            ignoreMissing = false;
        }
        var request = new genTypes.MultiDeleteDocumentsRequest;
        request.ids = documentIds;
        request.ignoreMissing = ignoreMissing;
        return original.apply(this, [request]);
    };
})();

(function() {
    var original = RelevancedClient.prototype.joinCentroid;
    RelevancedClient.prototype.joinCentroid = function(centroidId, ignoreMissing) {
        if (!_.isBoolean(ignoreMissing)) {
            ignoreMissing = false;
        }
        var request = new genTypes.JoinCentroidRequest;
        request.id = centroidId;
        request.ignoreMissing = ignoreMissing;
        return original.apply(this, [request]);
    };
})();

(function() {
    var original = RelevancedClient.prototype.multiJoinCentroids;
    RelevancedClient.prototype.multiJoinCentroids = function(centroidIds, ignoreMissing) {
        if (!_.isBoolean(ignoreMissing)) {
            ignoreMissing = false;
        }
        var request = new genTypes.MultiJoinCentroidsRequest;
        request.ids = centroidIds;
        request.ignoreMissing = ignoreMissing;
        return original.apply(this, [request]);
    };
})();

_.each([
    'Language',
    'ECentroidDoesNotExist',
    'ECentroidAlreadyExists',
    'EDocumentDoesNotExist',
    'EDocumentAlreadyExists',
    'EDocumentAlreadyInCentroid',
    'EDocumentNotInCentroid'
], function(prop) {
    RelevancedClient[prop] = genTypes[prop];
});

module.exports = {
    RelevancedClient: RelevancedClient,
    Language: genTypes.Language
};

