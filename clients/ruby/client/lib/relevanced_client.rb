$:.push('gen-rb')

require 'thrift'

module RelevancedClient

    require File.join(File.dirname(__FILE__), 'gen-rb', 'relevanced')
    require File.join(File.dirname(__FILE__), 'gen-rb', 'relevanced_protocol_types')

    class Client
        def initialize(host, port)
            @host = host
            @port = port
            @thrift_transport = Thrift::BufferedTransport.new(
                Thrift::Socket.new(host, port)
            )
            @thrift_protocol = Thrift::BinaryProtocol.new(
                @thrift_transport
            )
            @thrift_client = Relevanced::Client.new(@thrift_protocol)
            @thrift_transport.open()
        end

        def add_document_to_centroid(centroid_id, document_id, ignore_already_in_centroid=false)
            add_documents_to_centroid(centroid_id, [document_id], ignore_already_in_centroid)
        end

        def add_documents_to_centroid(centroid_id, document_ids, ignore_already_in_centroid=false)
            request = AddDocumentsToCentroidRequest.new
            request.centroidId = centroid_id
            request.documentIds = document_ids
            request.ignoreAlreadyInCentroid = ignore_already_in_centroid
            @thrift_client.addDocumentsToCentroid(request)
        end

        def create_document(text, lang=Language::EN)
            @thrift_client.createDocument(text, lang)
        end

        def create_document_with_id(document_id, text, lang=Language::EN)
            @thrift_client.createDocumentWithID(
                document_id, text, lang
            )
        end

        def get_server_metadata()
            @thrift_client.getServerMetadata()
        end

        def get_centroid_similarity(centroid_1_id, centroid_2_id)
            @thrift_client.getCentroidSimilarity(
                centroid_1_id, centroid_2_id
            )
        end

        def get_document_similarity(centroid_id, document_id)
            @thrift_client.getDocumentSimilarity(centroid_id, document_id)
        end

        def get_text_similarity(centroid_id, text, lang=Language::EN)
            @thrift_client.getTextSimilarity(centroid_id, text, lang)
        end

        def create_centroid(centroid_id, ignore_existing=false)
            request = CreateCentroidRequest.new
            request.id = centroid_id
            request.ignoreExisting = ignore_existing
            @thrift_client.createCentroid(request)
        end

        def delete_centroid(centroid_id, ignore_missing=false)
            request = DeleteCentroidRequest.new
            request.id = centroid_id
            request.ignoreMissing = ignore_missing
            @thrift_client.deleteCentroid(request)
        end

        def delete_document(document_id, ignore_missing=false)
            request = DeleteDocumentRequest.new
            request.id = document_id
            request.ignoreMissing = ignore_missing
            @thrift_client.deleteDocument(request)
        end

        def join_centroid(centroid_id)
            request = JoinCentroidRequest.new
            request.id = centroid_id
            @thrift_client.joinCentroid(request)
        end

        def multi_create_centroids(centroid_ids, ignore_existing=false)
            request = MultiCreateCentroidsRequest.new
            request.ids = centroid_ids
            request.ignoreExisting = ignore_existing
            @thrift_client.multiCreateCentroids(request)
        end

        def multi_delete_centroids(centroid_ids, ignore_missing=false)
            request = MultiDeleteCentroidsRequest.new
            request.ids = centroid_ids
            request.ignoreMissing = ignore_missing
            @thrift_client.multiDeleteCentroids(request)
        end

        def multi_delete_documents(document_ids, ignore_missing=false)
            request = MultiDeleteDocumentsRequest.new
            request.ids = document_ids
            request.ignoreMissing = ignore_missing
            @thrift_client.multiDeleteDocuments(request)
        end

        def multi_get_document_similarity(centroid_id_list, document_id)
            @thrift_client.multiGetDocumentSimilarity(
                centroid_id_list, document_id
            )
        end

        def multi_get_text_similarity(centroid_id_list, text, lang=Language::EN)
            @thrift_client.multiGetTextSimilarity(
                centroid_id_list, text, lang
            )
        end

        def multi_join_centroids(centroid_ids)
            request = MultiJoinCentroidsRequest.new
            request.ids = centroid_ids
            @thrift_client.multiJoinCentroids(request)
        end

        def list_all_centroids()
            @thrift_client.listAllCentroids()
        end

        def list_all_documents()
            @thrift_client.listAllDocuments()
        end

        def list_all_documents_for_centroid(centroid_id)
            @thrift_client.listAllDocumentsForCentroid(centroid_id)
        end

        def list_centroid_document_range(centroid_id, offset, count)
            @thrift_client.listCentroidDocumentRange(centroid_id, offset, count)
        end

        def list_centroid_document_range_from_id(centroid_id, document_id, count)
            @thrift_client.listCentroidDocumentRangeFromID(centroid_id, document_id, count)
        end


        def list_centroid_range(offset, count)
            @thrift_client.listCentroidRange(offset, count)
        end

        def list_centroid_range_from_id(centroid_id, count)
            @thrift_client.listCentroidRangeFromID(centroid_id, count)
        end

        def list_document_range(offset, count)
            @thrift_client.listDocumentRange(offset, count)
        end

        def list_document_range_from_id(document_id, count)
            @thrift_client.listDocumentRangeFromID(document_id, count)
        end

        def list_unused_documents(limit)
            @thrift_client.listUnusedDocuments(limit)
        end

        def ping()
            @thrift_client.ping()
        end

        def remove_document_from_centroid(centroid_id, document_id, ignore_not_in_centroid=false)
            remove_documents_from_centroid(centroid_id, [document_id], ignore_not_in_centroid)
        end

        def remove_documents_from_centroid(centroid_id, document_ids, ignore_not_in_centroid=false)
            request = RemoveDocumentsFromCentroidRequest.new
            request.centroidId = centroid_id
            request.documentIds = document_ids
            request.ignoreNotInCentroid = ignore_not_in_centroid
            @thrift_client.removeDocumentsFromCentroid(request)
        end

    end
end
