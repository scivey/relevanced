Title: Concepts

relevanced contains three main entities: vocabularies, documents and centroids.

## Vocabularies

A vocabulary is the collection of words used to turn documents into vector representations.  Word aren't added to a vocabulary directly: instead, it consists of the set of all words in the documents it contains.

relevanced starts with a single global vocabulary, named "default."  You don't necessarily need to create more of them -- it depends on the use case.

## Documents
A document is any blob of text.  Internally, relevanced stores documents as mappings of stemmed non-stopword terms to normalized frequencies.  Text like this:

    The cat foolishly bit the dog.  The dog bit the cat back.

Is first turned into something like the following:

    cat     2
    dog     2
    bit     2
    foolish 1
    back    1

And then divided by overall term count so that the values sum to 1:

    cat     0.25
    dog     0.25
    bit     0.25
    foolish 0.125
    back    0.125

This normalization prevents longer documents from being given too much weight.

Ultimately, the values of these terms determine the document's position in the vector space of a given vocabulary.

## Centroids
A centroid is a collection of documents, scoped to a particular vocabulary.  It represents the "center" of those documents' positions in the vector space defined by the parent vocabulary.

