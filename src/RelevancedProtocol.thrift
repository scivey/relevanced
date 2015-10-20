namespace cpp2 relevanced.thrift_protocol
namespace java org.relevanced.client.gen_thrift_protocol

struct WordVectorDTO {
    1: required double magnitude;
    2: required double documentWeight;
    3: required map<string, double> scores;
}

struct ScoredWordDTO {
    1: required binary wordBuff;
    2: required double score;
}

struct CentroidMetadataDTO {
    1: required string id;
    2: required i64 created;
    3: required i64 lastDocumentChange;
    4: required i64 lastCalculated;
}

struct CentroidDTO {
    1: required string id;
    2: required WordVectorDTO wordVector;
}

struct ProcessedDocumentMetadataDTO {
    1: required string id;
    2: required string sha1Hash;
    3: required i64 created;
    4: required i64 updated;
}

struct ProcessedDocumentPersistenceDTO {
    1: required ProcessedDocumentMetadataDTO metadata;
    2: required list<ScoredWordDTO> scoredWords;
    3: required double magnitude;
}

struct ProcessedDocumentDTO {
    1: required ProcessedDocumentMetadataDTO metadata;
    2: required WordVectorDTO wordVector;
}

struct GetDocumentMetadataResponse {
    1: required ProcessedDocumentMetadataDTO metadata;
}

struct GetFullDocumentResponse {
    1: required ProcessedDocumentDTO document;
}

struct GetCentroidMetadataResponse {
    1: required CentroidMetadataDTO metadata;
}

struct MultiSimilarityResponse {
    1: required map<string, double> scores;
}

struct ListCentroidDocumentsResponse {
    1: required list<string> documents;
}

struct ListDocumentsResponse {
    1: required list<string> documents;
}

struct ListCentroidsResponse {
    1: required list<string> centroids;
}

struct CreateDocumentResponse {
    1: required string id;
}

struct DeleteDocumentResponse {
    1: required string id;
}

struct CreateCentroidResponse {
    1: required string id;
}

struct DeleteCentroidResponse {
    1: required string id;
}

struct AddDocumentToCentroidResponse {
    1: required string centroidId;
    2: required string documentId;
}

struct RemoveDocumentFromCentroidResponse {
    1: required string centroidId;
    2: required string documentId;
}

struct JoinCentroidResponse {
    1: required string id;
    2: required bool recalculated;
}

exception ECentroidDoesNotExist {
    1: string id;
    2: string message;
}

exception ECentroidAlreadyExists {
    1: string id;
    2: string message;
}

exception EDocumentDoesNotExist {
    1: string id;
    2: string message;
}

exception EDocumentAlreadyExists {
    1: string id;
    2: string message;
}

exception EDocumentNotInCentroid {
    1: string documentId;
    2: string centroidId;
    3: string message;
}

exception EDocumentAlreadyInCentroid {
    1: string documentId;
    2: string centroidId;
    3: string message;
}

service Relevanced {
    void ping(),
    map<string, string> getServerMetadata(),
    double getDocumentSimilarity(1: string centroidId, 2: string docId) throws (1: ECentroidDoesNotExist centroidErr, 2: EDocumentDoesNotExist docErr),
    MultiSimilarityResponse multiGetTextSimilarity(1: list<string> centroidIds, 2: string text) throws (1: ECentroidDoesNotExist err),
    double getTextSimilarity(1: string centroidId, 2: string text) throws (1: ECentroidDoesNotExist err),
    double getCentroidSimilarity(1: string centroid1Id, 2: string centroid2Id) throws (1: ECentroidDoesNotExist err),
    CreateDocumentResponse createDocument(1: string text),
    CreateDocumentResponse createDocumentWithID(1: string id, 2: string text) throws (1: EDocumentAlreadyExists err),
    DeleteDocumentResponse deleteDocument(1: string id) throws (1: EDocumentDoesNotExist err),
    GetDocumentMetadataResponse getDocumentMetadata(1: string id) throws (1: EDocumentDoesNotExist err),
    CreateCentroidResponse createCentroid(1: string centroidId) throws (1: ECentroidAlreadyExists err),
    DeleteCentroidResponse deleteCentroid(1: string centroidId) throws (1: ECentroidDoesNotExist err),
    ListCentroidDocumentsResponse listAllDocumentsForCentroid(1: string centroidId) throws (1: ECentroidDoesNotExist err),
    AddDocumentToCentroidResponse addDocumentToCentroid(1: string centroidId, 2: string docId) throws (1: ECentroidDoesNotExist centroidErr, 2: EDocumentDoesNotExist docErr, 3: EDocumentAlreadyInCentroid bothErr),
    RemoveDocumentFromCentroidResponse removeDocumentFromCentroid(1: string centroidId, 2: string docId) throws (1: ECentroidDoesNotExist centroidErr, 2: EDocumentDoesNotExist docErr, 3: EDocumentNotInCentroid bothErr),
    JoinCentroidResponse joinCentroid(1: string centroidId) throws (1: ECentroidDoesNotExist err),
    ListCentroidsResponse listAllCentroids(),
    ListDocumentsResponse listAllDocuments(),
    void debugEraseAllData(),
    CentroidDTO debugGetFullCentroid(1: string centroidId) throws (1: ECentroidDoesNotExist err),
    ProcessedDocumentDTO debugGetFullProcessedDocument(1: string documentId) throws (1: EDocumentDoesNotExist err)
}
