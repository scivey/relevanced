namespace cpp2 services

service Relevance {
    void ping(),
    double getRelevanceForDoc(1: string collectionId, 2: string docId),
    double getRelevanceForText(1: string collectionId, 2: string text),
    string createDocument(1: string text),
    bool createDocumentWithID(1: string id, 2: string text),
    bool deleteDocument(1: string id),
    string getDocument(1: string id),
    bool createCollection(1: string collectionId),
    bool deleteCollection(1: string collectionId),
    list<string> listCollectionDocuments(1: string collectionId),
    bool addPositiveDocumentToCollection(1: string collectionId, 2: string docId)
    bool addNegativeDocumentToCollection(1: string collectionId, 2: string docId)
    bool removeDocumentFromCollection(1: string collectionId, 2: string docId)
    string addNewPositiveDocumentToCollection(1: string collectionId, 2: string text),
    string addNewNegativeDocumentToCollection(1: string collectionId, 2: string text),
    bool recompute(1: string collectionId),
    list<string> listCollections(),
    list<string> listDocuments(),
    list<string> listUnassociatedDocuments(),
    i64 getCollectionSize(1: string collectionId)
}
