namespace cpp2 relevanced.thrift_protocol
namespace java org.relevanced.client.gen_thrift_protocol

struct WordVectorDTO {
    1: required double magnitude;
    2: required double documentWeight;
    3: required map<string, double> scores;
}

struct CentroidDTO {
    1: required string id;
    2: required WordVectorDTO wordVector;
}

struct ProcessedDocumentDTO {
    1: required string id;
    2: required WordVectorDTO wordVector;
    3: required string sha1Hash;
    4: required i64 created;
    5: required i64 updated;
}

enum StatusCode {
    OK = 0,
    CENTROID_DOES_NOT_EXIST = 1,
    CENTROID_ALREADY_EXISTS = 2,
    DOCUMENT_DOES_NOT_EXIST = 3,
    DOCUMENT_ALREADY_EXISTS = 4,
    UNKNOWN_EXCEPTION = 5,
    DOCUMENT_ALREADY_IN_CENTROID = 6,
    DOCUMENT_NOT_IN_CENTROID = 7,
    OUT_OF_MEMORY = 8,
    NO_DISK_SPACE = 9
}

struct Status {
    1: required StatusCode code;
    2: required string message;
}

struct GetDocumentResponse {
    1: required Status status;
    2: required string document;
}

struct SimilarityResponse {
    1: required Status status;
    2: required double similarity;
}

struct MultiSimilarityResponse {
    1: required Status status;
    2: required map<string, double> scores;
}

struct GetCentroidSizeResponse {
    1: required Status status;
    2: required i32 size;
}

struct ListCentroidDocumentsResponse {
    1: required Status status;
    2: required list<string> documents;
}

struct CrudResponse {
    1: required string id;
}

exception TCentroidDoesNotExist {
    1: string id;
    2: string message;
}

exception TCentroidAlreadyExists {
    1: string id;
    2: string message;
}

exception TDocumentDoesNotExist {
    1: string id;
    2: string message;
}

exception TDocumentAlreadyExists {
    1: string id;
    2: string message;
}

exception TDocumentNotInCentroid {
    1: string documentId;
    2: string centroidId;
    3: string message;
}

exception TDocumentAlreadyInCentroid {
    1: string documentId;
    2: string centroidId;
    3: string message;
}

service Relevanced {
    void ping(),
    map<string, string> getServerMetadata(),
    SimilarityResponse getDocumentSimilarity(1: string centroidId, 2: string docId) throws (1: TCentroidDoesNotExist centroidErr, 2: TDocumentDoesNotExist docErr),
    MultiSimilarityResponse multiGetTextSimilarity(1: list<string> centroidIds, 2: string text) throws (1: TCentroidDoesNotExist err),
    SimilarityResponse getTextSimilarity(1: string centroidId, 2: string text) throws (1: TCentroidDoesNotExist err),
    SimilarityResponse getCentroidSimilarity(1: string centroid1Id, 2: string centroid2Id) throws (1: TCentroidDoesNotExist err),
    CrudResponse createDocument(1: string text),
    CrudResponse createDocumentWithID(1: string id, 2: string text) throws (1: TDocumentAlreadyExists err),
    CrudResponse deleteDocument(1: string id) throws(1: TDocumentDoesNotExist err),
    GetDocumentResponse getDocument(1: string id) throws(1: TDocumentDoesNotExist err),
    CrudResponse createCentroid(1: string centroidId) throws(1: TCentroidAlreadyExists err),
    CrudResponse deleteCentroid(1: string centroidId) throws(1: TCentroidDoesNotExist err),
    ListCentroidDocumentsResponse listAllDocumentsForCentroid(1: string centroidId) throws (1: TCentroidDoesNotExist err),
    CrudResponse addDocumentToCentroid(1: string centroidId, 2: string docId) throws (1: TCentroidDoesNotExist centroidErr, 2: TDocumentDoesNotExist docErr, 3: TDocumentAlreadyInCentroid bothErr),
    CrudResponse removeDocumentFromCentroid(1: string centroidId, 2: string docId) throws (1: TCentroidDoesNotExist centroidErr, 2: TDocumentDoesNotExist docErr, 3: TDocumentNotInCentroid bothErr),
    bool joinCentroid(1: string centroidId) throws (1: TCentroidDoesNotExist err),
    list<string> listAllCentroids(),
    list<string> listAllDocuments()
}
