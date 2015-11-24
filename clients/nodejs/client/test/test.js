var RelevancedClient = require('../').RelevancedClient;
var _ = require('lodash');
var log = _.bind(console.log, console);
var assert = require('chai').assert;
var Promise = require('bluebird');

var eraseAllCentroids = function(client) {
    return client
        .listAllCentroids()
        .then(function(response) {
            var deletions = _.map(response.centroids, function(id) {
                var ignoreMissing = true;
                return client.deleteCentroid(id, ignoreMissing);
            });
            return Promise.all(deletions);
        });
}

var eraseAllDocuments = function(client) {
    return client
        .listAllDocuments()
        .then(function(response) {
            var deletions = _.map(response.documents, function(id) {
                var ignoreMissing = true;
                return client.deleteDocument(id, ignoreMissing);
            });
            return Promise.all(deletions);
        });
}

var eraseEverything = function(client) {
    return Promise.all([
        eraseAllCentroids(client), eraseAllDocuments(client)
    ]);
}

describe('RelevancedClient', function() {
    before(function(done) {
        var self = this;
        RelevancedClient
            .connect('localhost', 8097)
            .then(function(client) {
                self.client = client;
                done();
            }).catch(done);
    });
    beforeEach(function(done) {
        eraseEverything(this.client)
            .then(function() {
                done();
            }).catch(done);
    });
    afterEach(function(done) {
        eraseEverything(this.client)
            .then(function() {
                done();
            }).catch(done);
    })
    describe('centroid CRUD', function() {
        it('creation and deletion work', function(done) {
            var client = this.client;
            Promise.all([
                client.createCentroid('c-1'),
                client.createCentroid('c-2'),
                client.multiCreateCentroids(['c-3', 'c-4'])
            ]).then(function() {
                return client.listAllCentroids();
            })
            .then(function(listCentroids) {
                assert.deepEqual(
                    ['c-1', 'c-2', 'c-3', 'c-4'],
                    listCentroids.centroids
                );
                return Promise.all([
                    client.deleteCentroid('c-1'),
                    client.multiDeleteCentroids([
                        'c-2', 'c-4'
                    ])
                ]);
            }).then(function() {
                return client.listAllCentroids();
            }).then(function(listCentroids) {
                assert.deepEqual(
                    ['c-3'], listCentroids.centroids
                );
                done();
            }).catch(done);
        });
    });
    describe('document CRUD', function() {
        it('creation and deletion work', function(done) {
            var client = this.client;
            var creations = _.map(_.range(0, 6), function(id) {
                id = 'doc-id-' + id;
                return client.createDocumentWithID(id, 'stub text');
            });
            Promise.all(creations)
                .then(function() {
                    return client.listAllDocuments();
                })
                .then(function(res) {
                    assert.deepEqual(
                        ['doc-id-0', 'doc-id-1', 'doc-id-2', 'doc-id-3', 'doc-id-4', 'doc-id-5'],
                        res.documents
                    );
                    return Promise.all([
                        client.createDocumentWithID('other-doc-1', 'some text'),
                        client.createDocumentWithID('other-doc-2', 'some text'),
                        client.deleteDocument('doc-id-2'),
                        client.multiDeleteDocuments(['doc-id-3', 'doc-id-5'])
                    ]);
                })
                .then(function() {
                    return client.listAllDocuments();
                })
                .then(function(res) {
                    assert.deepEqual(
                        ['doc-id-0', 'doc-id-1', 'doc-id-4', 'other-doc-1', 'other-doc-2'],
                        res.documents
                    );
                    done();
                }).catch(done);
        });
    });
    describe('centroid document operations', function() {
        beforeEach(function(done) {
            var client = this.client;
            var docIds = ['doc-1', 'doc-2', 'doc-3', 'doc-4', 'doc-5', 'doc-6', 'doc-7'];
            var centroidIds = ['cx-1', 'cx-2', 'cx-3'];
            var docCreations = Promise.all(_.map(docIds, function(id) {
                return client.createDocumentWithID(
                    id, 'some text'
                );
            }));
            Promise.all([
                client.multiCreateCentroids(centroidIds),
                docCreations
            ]).then(function() {
                done();
            }).catch(done);
        });
        it('works', function(done) {
            var client = this.client;
            client
                .listAllDocumentsForCentroid('cx-1')
                .then(function(response) {
                    assert.deepEqual([], response.documents);
                    return Promise.all([
                        client.addDocumentsToCentroid(
                            'cx-1', ['doc-2', 'doc-4', 'doc-5']
                        ),
                        client.addDocumentsToCentroid(
                            'cx-3', ['doc-5', 'doc-6', 'doc-7']
                        )
                    ]);
                })
                .then(function() {
                    return Promise.all([
                        client.listAllDocumentsForCentroid('cx-1'),
                        client.listAllDocumentsForCentroid('cx-2'),
                        client.listAllDocumentsForCentroid('cx-3')
                    ]);
                })
                .then(function(lists) {
                    assert.deepEqual(
                        ['doc-2', 'doc-4', 'doc-5'], lists[0].documents
                    );
                    assert.deepEqual([], lists[1].documents);
                    assert.deepEqual(
                        ['doc-5', 'doc-6', 'doc-7'], lists[2].documents
                    );
                    return Promise.all([
                        client.removeDocumentsFromCentroid(
                            'cx-1', ['doc-4', 'doc-5']
                        ),
                        client.addDocumentsToCentroid(
                            'cx-2', ['doc-1', 'doc-3']
                        ),
                        client.removeDocumentsFromCentroid(
                            'cx-3', ['doc-6']
                        )
                    ]);
                }).then(function() {
                    return Promise.all([
                        client.listAllDocumentsForCentroid('cx-1'),
                        client.listAllDocumentsForCentroid('cx-2'),
                        client.listAllDocumentsForCentroid('cx-3')
                    ]);
                }).then(function(lists) {
                    assert.deepEqual(
                        ['doc-2'], lists[0].documents
                    );
                    assert.deepEqual(
                        ['doc-1', 'doc-3'], lists[1].documents
                    );
                    assert.deepEqual(
                        ['doc-5', 'doc-7'], lists[2].documents
                    );
                    done();
                }).catch(function(err) {
                    console.log('err', err);
                    done(err);
                });
        });
    });
    describe('similarity scoring', function() {
        var data;
        beforeEach(function(done) {
            data = {
                'monkeys': {
                    'doc-monkeys-1': 'monkey gorilla ape banana cat',
                    'doc-monkeys-2': 'gorilla gorilla gorilla fish banana',
                    'doc-monkeys-3': 'ape lemur gorilla monkey grill'
                },
                'cars': {
                    'doc-cars-1': 'engine motor wheel gear clutch',
                    'doc-cars-2': 'motor motor nascar left wheel',
                    'doc-cars-3': 'mudflap column engine engine engine'
                }
            };
            var client = this.client;
            var creations = [];
            _.each(data, function(docSet, centroidId) {
                creations.push(client.createCentroid(centroidId));
                _.each(docSet, function(docText, docId) {
                    creations.push(client.createDocumentWithID(docId, docText));
                });
            });
            Promise.all(creations)
                .then(function() {
                    return Promise.all(_.map(data, function(docSet, centroid) {
                        return client.addDocumentsToCentroid(
                            centroid, _.keys(docSet)
                        );
                    }));
                }).then(function() {
                    return Promise.all([
                        client.joinCentroid('monkeys'),
                        client.joinCentroid('cars')
                    ]);
                }).then(function() {
                    done();
                }).catch(done);
        });
        it('joinCentroid works', function(done) {
            var client = this.client;
            client.createDocumentWithID(
                'another-monkey-doc', 'monkey monkey ape banana banana'
            ).then(function() {
                return client.addDocumentsToCentroid(
                    'monkeys', ['another-monkey-doc']
                );
            }).then(function() {
                return client.joinCentroid('monkeys');
            }).then(function(res) {
                assert.equal('monkeys', res.id);
                done();
            }).catch(done);
        });
        it('multiJoinCentroids works', function(done) {
            var client = this.client;
            Promise.all([
                client.createDocumentWithID('another-monkey-doc', 'monkey banana'),
                client.createDocumentWithID('another-car-doc', 'engine engine')
            ]).then(function() {
                return Promise.all([
                    client.addDocumentsToCentroid('monkeys', ['another-monkey-doc']),
                    client.addDocumentsToCentroid('cars', ['another-car-doc'])
                ]);
            }).then(function() {
                return client.multiJoinCentroids(['monkeys', 'cars']);
            }).then(function(res) {
                assert.deepEqual(
                    ['monkeys', 'cars'], res.ids
                );
                done();
            }).catch(done);
        });
        it('getTextSimilarity works', function(done) {
            var client = this.client;
            var testText = 'ape monkey monkey gorilla engine';
            Promise.all([
                client.getTextSimilarity('monkeys', testText),
                client.getTextSimilarity('cars', testText)
            ]).then(function(results) {
                assert.isNumber(results[0]);
                assert.isNumber(results[1]);
                assert.isAbove(results[0], results[1]);
                assert.isAbove(results[0], 0);
                assert.isAbove(results[1], 0);
                assert.isBelow(results[0], 1.0001);
                assert.isBelow(results[1], 1.0001);
                done();
            }).catch(function(err) {
                console.log(err);
                done(err);
            });
        });
    });
});