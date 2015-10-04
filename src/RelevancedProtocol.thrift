namespace cpp2 relevanced.thrift_protocol

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

enum RelevanceStatus {
    OK = 0,
    CENTROID_DOES_NOT_EXIST = 1,
    CENTROID_ALREADY_EXISTS = 2,
    DOCUMENT_DOES_NOT_EXIST = 3,
    DOCUMENT_ALREADY_EXISTS = 4,
    UNKNOWN_EXCEPTION = 5
}

struct GetDocumentResponse {
    1: required RelevanceStatus status;
    2: required string document;
}

struct DocumentRelevanceResponse {
    1: required RelevanceStatus status;
    2: required double relevance;
}

struct DocumentMultiRelevanceResponse {
    1: required RelevanceStatus status;
    2: required map<string, double> scores;
}

struct GetCentroidSizeResponse {
    1: required RelevanceStatus status;
    2: required i32 size;
}

struct ListCentroidDocumentsResponse {
    1: required RelevanceStatus status;
    2: required list<string> documents;
}

struct CrudResponse {
    1: required RelevanceStatus status;
    2: required string created;
}

service Relevanced {
    void ping(),
    DocumentRelevanceResponse getDocumentSimilarity(1: string centroidId, 2: string docId),
    DocumentMultiRelevanceResponse multiGetTextSimilarity(1: list<string> centroidIds, 2: string text),
    double getCentroidSimilarity(1: string centroid1Id, 2: string centroid2Id),
    DocumentRelevanceResponse getTextSimilarity(1: string centroidId, 2: string text),
    CrudResponse createDocument(1: string text),
    CrudResponse createDocumentWithID(1: string id, 2: string text),
    CrudResponse deleteDocument(1: string id),
    GetDocumentResponse getDocument(1: string id),
    CrudResponse createCentroid(1: string centroidId),
    CrudResponse deleteCentroid(1: string centroidId),
    ListCentroidDocumentsResponse listAllDocumentsForCentroid(1: string centroidId),
    CrudResponse addDocumentToCentroid(1: string centroidId, 2: string docId),
    CrudResponse removeDocumentFromCentroid(1: string centroidId, 2: string docId),
    bool recomputeCentroid(1: string centroidId),
    list<string> listAllCentroids(),
    list<string> listAllDocuments()
}
