var RelevancedClient = require('../').RelevancedClient;
var _ = require('lodash');
var log = _.bind(console.log, console);

var main = function() {
    var client = new RelevancedClient('localhost', 8097);
    client.getServerMetadata().then(function(metadata) {
        log('server metadata: ');
        log(metadata);
    }).catch(log);
    client.listAllCentroids().then(function(centroids) {
        log("Centroid count: ", centroids.length);
        log(centroids);
        centroids = _.filter(centroids, function(x) {
            return x.indexOf('wiki') !== -1;
        });
        client.listAllDocuments().then(function(documents) {
            log("document count: ", documents.length);
            log(documents.slice(0, 10));
            var centroid = centroids[0];
            var doc = documents[5];
            client.getDocumentSimilarity(centroid, doc).then(function(similarity) {
                log('got similarity for ', doc, ' against ', centroid);
                log('result: ', similarity);
            }).catch(log);
        }).catch(log);
    }).catch(log);
};

main();
