$:.push('gen-rb')

require 'thrift'
require 'relevanced'

module RelevancedClient
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

        def ping()
            @thrift_client.ping()
        end

        def get_server_metadata()
            @thrift_client.getServerMetadata()
        end

        def get_document_similarity(centroid_id, document_id)
            @thrift_client.getDocumentSimilarity(centroid_id, document_id)
        end

        def multi_get_text_similarity(centroid_id_list, text)
            @thrift_client.multiGetTextSimilarity(
                centroid_id_list, text
            )
        end

        def get_text_similarity(centroid_id, text)
            @thrift_client.getTextSimilarity(centroid_id, text)
        end


        def get_centroid_similarity(centroid_1_id, centroid_2_id)
            @thrift_client.getCentroidSimilarity(
                centroid_1_id, centroid_2_id
            )
        end

        def create_document(text)
            @thrift_client.createDocument(text)
        end

        def create_document_with_id(document_id, text)
            @thrift_client.createDocumentWithId(
                document_id, text
            )
        end

        def delete_document(document_id)
            @thrift_client.deleteDocument(document_id)
        end

        def create_centroid(centroid_id)
            @thrift_client.createCentroid(centroid_id)
        end

        def delete_centroid(centroid_id)
            @thrift_client.deleteCentroid(centroid_id)
        end

        def list_all_documents_for_centroid(centroid_id)
            @thrift_client.listAllDocumentsForCentroid(centroid_id)
        end

        def add_document_to_centroid(centroid_id, document_id)
            @thrift_client.addDocumentToCentroid(
                centroid_id, document_id
            )
        end

        def remove_document_from_centroid(centroid_id, document_id)
            @thrift_client.removeDocumentFromCentroid(
                centroid_id, document_id
            )
        end

        def join_centroid(centroid_id)
            @thrift_client.joinCentroid(centroid_id)
        end

        def list_all_centroids()
            @thrift_client.listAllCentroids()
        end

        def list_all_documents()
            @thrift_client.listAllDocuments()
        end

    end
end
