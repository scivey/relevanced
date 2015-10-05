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
}

enum StatusCode {
    OK = 0,
    CENTROID_DOES_NOT_EXIST = 1,
    CENTROID_ALREADY_EXISTS = 2,
    DOCUMENT_DOES_NOT_EXIST = 3,
    DOCUMENT_ALREADY_EXISTS = 4,
    UNKNOWN_EXCEPTION = 5
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
    1: required Status status;
    2: required string created;
}

service Relevanced {
    void ping(),
    SimilarityResponse getDocumentSimilarity(1: string centroidId, 2: string docId),
    MultiSimilarityResponse multiGetTextSimilarity(1: list<string> centroidIds, 2: string text),
    SimilarityResponse getTextSimilarity(1: string centroidId, 2: string text),
    SimilarityResponse getCentroidSimilarity(1: string centroid1Id, 2: string centroid2Id),
    CrudResponse createDocument(1: string text),
    CrudResponse createDocumentWithID(1: string id, 2: string text),
    CrudResponse deleteDocument(1: string id),
    GetDocumentResponse getDocument(1: string id),
    CrudResponse createCentroid(1: string centroidId),
    CrudResponse deleteCentroid(1: string centroidId),
    ListCentroidDocumentsResponse listAllDocumentsForCentroid(1: string centroidId),
    CrudResponse addDocumentToCentroid(1: string centroidId, 2: string docId),
    CrudResponse removeDocumentFromCentroid(1: string centroidId, 2: string docId),
    Status recomputeCentroid(1: string centroidId),
    list<string> listAllCentroids(),
    list<string> listAllDocuments()
}
