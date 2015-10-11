var RelevancedClient = require('../').RelevancedClient;
var _ = require('lodash');
var log = _.bind(console.log, console);

var main = function() {
    RelevancedClient.connect('localhost', 8097).then(function(client) {
        client.getServerMetadata().then(function(metadata) {
            log('server metadata: ');
            log(metadata);
            client.listAllCentroids().then(function(result) {
                var centroids = result.centroids;
                log("Centroid count: ", centroids.length);
                log(centroids);
                centroids = _.filter(centroids, function(x) {
                    return x.indexOf('wiki') !== -1;
                });
                client.listAllDocuments().then(function(docRes) {
                    var documents = docRes.documents;
                    log("document count: ", documents.length);
                    log(documents.slice(0, 10));
                    var centroid = centroids[0];
                    var doc = documents[5];
                    client.getDocumentSimilarity(centroid, doc).then(function(similarity) {
                        log('got similarity for ', doc, ' against ', centroid);
                        log('result: ', similarity);
                        client.disconnect();
                    }).catch(log);
                }).catch(log);
            }).catch(log);
        }).catch(log);
    }).catch(log);
};

main();
