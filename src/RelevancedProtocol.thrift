namespace cpp2 relevanced.thrift_protocol
namespace java org.relevanced.client.gen_thrift_protocol

cpp_include "unordered_map"

enum Language  {
    DE,
    EN,
    ES,
    FR,
    IT,
    RU,
    OTHER = 254
}

typedef map<string, double> (cpp.template = "std::unordered_map") stringToDoubleMap

struct WordVectorDTO {
    1: required double magnitude;
    2: required double documentWeight;
    3: required stringToDoubleMap scores;
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

struct CentroidSimilarityMatrix {
    1: required list<string> centroidIds;
    2: required list<list<double>> scores;
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

struct DeleteDocumentRequest {
    1: required string id;
    2: optional bool ignoreMissing;
}

struct DeleteDocumentResponse {
    1: required string id;
}

struct MultiDeleteDocumentsRequest {
    1: required list<string> ids;
    2: optional bool ignoreMissing;
}

struct MultiDeleteDocumentsResponse {
    1: required list<string> ids;
}


struct MultiDeleteCentroidsRequest {
    1: required list<string> ids;
    2: required bool ignoreMissing;
}

struct MultiDeleteCentroidsResponse {
    1: required list<string> ids;
}

struct CreateCentroidResponse {
    1: required string created;
}

struct CreateCentroidRequest {
    1: required string id;
    2: required bool ignoreExisting;
}

struct MultiCreateCentroidsRequest {
    1: required list<string> ids;
    2: required bool ignoreExisting;
}

struct MultiCreateCentroidsResponse {
    1: required list<string> created;
}

struct DeleteCentroidRequest {
    1: required string id;
    2: required bool ignoreMissing;
}

struct DeleteCentroidResponse {
    1: required string id;
}

struct AddDocumentsToCentroidRequest {
    1: required string centroidId;
    2: required list<string> documentIds;
    3: optional bool ignoreMissingDocument;
    4: optional bool ignoreMissingCentroid;
    5: optional bool ignoreAlreadyInCentroid;
}

struct AddDocumentsToCentroidResponse {
    1: required string centroidId;
    2: required list<string> documentIds;
    3: required list<bool> added;
}

struct RemoveDocumentsFromCentroidRequest {
    1: required string centroidId;
    2: required list<string> documentIds;
    3: optional bool ignoreMissingDocument;
    4: optional bool ignoreMissingCentroid;
    5: optional bool ignoreNotInCentroid;
}

struct RemoveDocumentsFromCentroidResponse {
    1: required string centroidId;
    2: required list<string> documentIds;
    3: required list<bool> removed;
}

struct MultiJoinCentroidsRequest {
    1: required list<string> ids;
    2: required bool ignoreMissing;
}

struct MultiJoinCentroidsResponse {
    1: required list<string> ids;
    2: required list<bool> recalculated;
}

struct JoinCentroidRequest {
    1: required string id;
    2: required bool ignoreMissing;
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
    MultiSimilarityResponse multiGetDocumentSimilarity(1: list<string> centroidIds, 2: string documentId) throws (1: ECentroidDoesNotExist centroidErr, 2: EDocumentDoesNotExist docErr),
    double getTextSimilarity(1: string centroidId, 2: string text, 3: Language lang) throws (1: ECentroidDoesNotExist err),
    MultiSimilarityResponse multiGetTextSimilarity(1: list<string> centroidIds, 2: string text, 3: Language lang) throws (1: ECentroidDoesNotExist err),
    double getCentroidSimilarity(1: string centroid1Id, 2: string centroid2Id) throws (1: ECentroidDoesNotExist err),
    CentroidSimilarityMatrix getCentroidSimilarityMatrix(1: list<string> centroidIds) throws (1: ECentroidDoesNotExist err),
    CreateDocumentResponse createDocument(1: string text, 2: Language language),
    CreateDocumentResponse createDocumentWithID(1: string id, 2: string text, 3: Language language) throws (1: EDocumentAlreadyExists err),
    DeleteDocumentResponse deleteDocument(1: DeleteDocumentRequest request) throws (1: EDocumentDoesNotExist err),
    MultiDeleteDocumentsResponse multiDeleteDocuments(1: MultiDeleteDocumentsRequest request) throws (1: EDocumentDoesNotExist err),
    GetDocumentMetadataResponse getDocumentMetadata(1: string id) throws (1: EDocumentDoesNotExist err),
    ListDocumentsResponse listAllDocuments(),
    ListDocumentsResponse listUnusedDocuments(1: i64 limit),
    ListDocumentsResponse listDocumentRange(1: i64 offset, 2: i64 count),
    ListDocumentsResponse listDocumentRangeFromID(1: string documentId, 2: i64 count),

    CreateCentroidResponse createCentroid(1: CreateCentroidRequest request) throws (1: ECentroidAlreadyExists err),
    MultiCreateCentroidsResponse multiCreateCentroids(1: MultiCreateCentroidsRequest request) throws (1: ECentroidAlreadyExists err),
    DeleteCentroidResponse deleteCentroid(1: DeleteCentroidRequest request) throws (1: ECentroidDoesNotExist err),
    MultiDeleteCentroidsResponse multiDeleteCentroids(1: MultiDeleteCentroidsRequest request) throws (1: ECentroidDoesNotExist err),
    AddDocumentsToCentroidResponse addDocumentsToCentroid(1: AddDocumentsToCentroidRequest request) throws (1: ECentroidDoesNotExist centroidErr, 2: EDocumentDoesNotExist docErr, 3: EDocumentAlreadyInCentroid bothErr),
    RemoveDocumentsFromCentroidResponse removeDocumentsFromCentroid(1: RemoveDocumentsFromCentroidRequest request) throws (1: ECentroidDoesNotExist centroidErr, 2: EDocumentDoesNotExist docErr, 3: EDocumentNotInCentroid bothErr),
    JoinCentroidResponse joinCentroid(1: JoinCentroidRequest request) throws (1: ECentroidDoesNotExist err),
    MultiJoinCentroidsResponse multiJoinCentroids(1: MultiJoinCentroidsRequest request) throws (1: ECentroidDoesNotExist err),
    ListCentroidsResponse listAllCentroids(),
    ListCentroidsResponse listCentroidRange(1: i64 offset, 2: i64 count),
    ListCentroidsResponse listCentroidRangeFromID(1: string centroidId, 2: i64 count),

    ListCentroidDocumentsResponse listAllDocumentsForCentroid(1: string centroidId) throws (1: ECentroidDoesNotExist err),
    ListCentroidDocumentsResponse listCentroidDocumentRange(1: string centroidId, 2: i64 offset, 3: i64 count) throws (1: ECentroidDoesNotExist err),
    ListCentroidDocumentsResponse listCentroidDocumentRangeFromID(1: string centroidId, 2: string documentId, 3: i64 count) throws (1: ECentroidDoesNotExist err),

    void debugEraseAllData(),
    CentroidDTO debugGetFullCentroid(1: string centroidId) throws (1: ECentroidDoesNotExist err),
    ProcessedDocumentDTO debugGetFullProcessedDocument(1: string documentId) throws (1: EDocumentDoesNotExist err)
}
