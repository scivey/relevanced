namespace cpp2 services

service Relevance {
    void ping(),
    double getRelevance(1: string collectionId, 2: string text),
    bool createCollection(1: string collectionId),
    bool addPositiveToCollection(1: string collectionId, 2: string textId, 3: string text),
    bool addNegativeToCollection(1: string collectionId, 2: string textId, 3: string text),
    bool recompute(1: string collectionId),
    list<string> listCollections(),
    i64 getCollectionSize(1: string collectionId)
}
