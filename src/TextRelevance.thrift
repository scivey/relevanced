namespace cpp2 services

enum RelevanceStatus {
    OK = 0,
    CLASSIFIER_DOES_NOT_EXIST = 1,
    CLASSIFIER_ALREADY_EXISTS = 2,
    DOCUMENT_DOES_NOT_EXIST = 3,
    DOCUMENT_ALREADY_EXISTS = 4
}

struct DocumentRelevanceResponse {
    1: required RelevanceStatus status;
    2: required double relevance;
}

struct GetClassifierSizeResponse {
    1: required RelevanceStatus status;
    2: required i32 size;
}

struct ListClassifierDocumentsResponse {
    1: required RelevanceStatus status;
    2: required list<string> documents;
}

struct CrudResponse {
    1: required RelevanceStatus status;
    2: required string created;
}

service Relevance {
    void ping(),
    DocumentRelevanceResponse getRelevanceForDoc(1: string classifierId, 2: string docId),
    DocumentRelevanceResponse getRelevanceForText(1: string classifierId, 2: string text),
    CrudResponse createDocument(1: string text),
    CrudResponse createDocumentWithID(1: string id, 2: string text),
    CrudResponse deleteDocument(1: string id),
    string getDocument(1: string id),
    CrudResponse createClassifier(1: string classifierId),
    CrudResponse deleteClassifier(1: string classifierId),
    ListClassifierDocumentsResponse listAllClassifierDocuments(1: string classifierId),
    CrudResponse addPositiveDocumentToClassifier(1: string classifierId, 2: string docId)
    CrudResponse addNegativeDocumentToClassifier(1: string classifierId, 2: string docId)
    CrudResponse removeDocumentFromClassifier(1: string classifierId, 2: string docId)
    bool recompute(1: string classifierId),
    list<string> listClassifiers(),
    list<string> listDocuments(),
    GetClassifierSizeResponse getClassifierSize(1: string classifierId)
}
