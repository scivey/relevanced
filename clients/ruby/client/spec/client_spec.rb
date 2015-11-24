require './lib/relevanced_client'

describe 'relevanced_client' do
    before(:all) do
        @client = RelevancedClient::Client.new('localhost', 8097)
    end
    before(:each) do
        existing_docs = @client.list_all_documents.documents
        existing_centroids = @client.list_all_centroids.centroids
        if existing_docs
            @client.multi_delete_documents(existing_docs)
        end
        if existing_centroids
            @client.multi_delete_centroids(existing_centroids)
        end
    end
    describe 'centroid CRUD' do
        it 'works' do
            @client.create_centroid('c-1')
            centroids = @client.list_all_centroids.centroids
            expect(centroids).to eq(['c-1'])

            @client.multi_create_centroids(['c-2', 'c-3'])
            centroids = @client.list_all_centroids.centroids
            expect(centroids).to eq(['c-1', 'c-2', 'c-3'])

            @client.multi_create_centroids(['c-4', 'c-5', 'c-6'])
            centroids = @client.list_all_centroids.centroids
            expect(centroids).to eq(['c-1', 'c-2', 'c-3', 'c-4', 'c-5', 'c-6'])

            @client.delete_centroid('c-2')
            centroids = @client.list_all_centroids.centroids
            expect(centroids).to eq(['c-1', 'c-3', 'c-4', 'c-5', 'c-6'])

            @client.multi_delete_centroids(['c-4', 'c-6'])
            centroids = @client.list_all_centroids.centroids
            expect(centroids).to eq(['c-1', 'c-3', 'c-5'])
        end
    end
    describe 'document CRUD' do
        it 'works' do
            ['d1', 'd2', 'd3', 'd4', 'd5'].each do |doc|
                @client.create_document_with_id(doc, 'some text')
            end
            docs = @client.list_all_documents.documents
            expect(docs).to eq(['d1', 'd2', 'd3', 'd4', 'd5'])

            @client.delete_document('d2')
            docs = @client.list_all_documents.documents
            expect(docs).to eq(['d1', 'd3', 'd4', 'd5'])

            @client.multi_delete_documents(['d3', 'd5'])
            docs = @client.list_all_documents.documents
            expect(docs).to eq(['d1', 'd4'])
        end
    end
    describe 'centroid-document CRUD' do
        before(:each) do
            ['d1', 'd2', 'd3', 'd4', 'd5'].each do |doc|
                @client.create_document_with_id(doc, 'some text')
            end
            @client.multi_create_centroids(['c1', 'c2', 'c3'])
        end
        it 'works' do
            docs = [
                @client.list_all_documents_for_centroid('c1').documents,
                @client.list_all_documents_for_centroid('c2').documents,
                @client.list_all_documents_for_centroid('c3').documents,
            ]
            expect(docs).to eq([[], [], []])
            @client.add_document_to_centroid('c1', 'd1')
            @client.add_document_to_centroid('c2', 'd3')
            @client.add_document_to_centroid('c2', 'd4')
            @client.add_document_to_centroid('c2', 'd5')


            docs = [
                @client.list_all_documents_for_centroid('c1').documents,
                @client.list_all_documents_for_centroid('c2').documents,
                @client.list_all_documents_for_centroid('c3').documents,
            ]
            expect(docs).to eq([['d1'], ['d3', 'd4', 'd5'], []])

            @client.add_documents_to_centroid('c3', ['d3', 'd4'])

            docs = [
                @client.list_all_documents_for_centroid('c1').documents,
                @client.list_all_documents_for_centroid('c2').documents,
                @client.list_all_documents_for_centroid('c3').documents,
            ]
            expect(docs).to eq([['d1'], ['d3', 'd4', 'd5'], ['d3', 'd4']])

            @client.remove_documents_from_centroid('c2', ['d3', 'd5'])

            docs = [
                @client.list_all_documents_for_centroid('c1').documents,
                @client.list_all_documents_for_centroid('c2').documents,
                @client.list_all_documents_for_centroid('c3').documents,
            ]
            expect(docs).to eq([['d1'], ['d4'], ['d3', 'd4']])

            @client.remove_document_from_centroid('c1', 'd1')
            docs = [
                @client.list_all_documents_for_centroid('c1').documents,
                @client.list_all_documents_for_centroid('c2').documents,
                @client.list_all_documents_for_centroid('c3').documents,
            ]
            expect(docs).to eq([[], ['d4'], ['d3', 'd4']])

        end
    end
    describe 'joining' do
        before(:each) do
            ['d1', 'd2', 'd3', 'd4', 'd5'].each do |doc|
                @client.create_document_with_id(doc, 'some text')
            end
            @client.multi_create_centroids(['c1', 'c2', 'c3'])
        end
        it 'works - single' do
            @client.add_documents_to_centroid('c2', ['d2', 'd3'])
            res = @client.join_centroid('c2')
            expect(res.id).to eq('c2')
        end
        it 'works - multi' do
            @client.add_documents_to_centroid('c2', ['d2', 'd3'])
            @client.add_documents_to_centroid('c3', ['d1'])
            res = @client.multi_join_centroids(['c2', 'c3'])
            expect(res.ids).to eq(['c2', 'c3'])
        end
    end
    describe 'similarity requests' do
        before(:each) do
            data = {
                "monkeys" => {
                    "d_monkeys_1" => 'monkey gorilla ape banana cat',
                    "d_monkeys_2" => 'gorilla gorilla gorilla fish banana',
                    "d_monkeys_3" => 'ape lemur gorilla monkey grill'
                },
                "cars" => {
                    "d_cars_1" => 'engine motor wheel gear clutch',
                    "d_cars_2" => 'motor motor nascar left wheel',
                    "d_cars_3" => 'mudflap column engine engine engine'
                }
            }
            @client.multi_create_centroids(data.keys)
            data.each do |centroid_id, docs|
                docs.each do |k, v|
                    @client.create_document_with_id(k, v)
                end
                @client.add_documents_to_centroid(centroid_id, docs.keys)
            end
            @client.multi_join_centroids(data.keys)
        end
        it 'works - single doc' do
            res1 = @client.get_document_similarity('monkeys', 'd_monkeys_1')
            res2 = @client.get_document_similarity('cars', 'd_monkeys_1')
            expect(res1).to be > res2
            expect(res1).to be > 0
            expect(res1).to be < 1.0001
            expect(res2).to be >= 0
            expect(res2).to be < 1.0001
        end
        it 'works - multi doc' do
            text = 'monkey lemur ape ape gear'
            @client.create_document_with_id('some-doc', text)
            res = @client.multi_get_document_similarity(['monkeys', 'cars'], 'some-doc')
            scores = res.scores
            expect(scores.keys).to eq(['cars', 'monkeys'])
            res1 = scores['monkeys']
            res2 = scores['cars']
            expect(res1).to be > res2
            expect(res1).to be > 0
            expect(res1).to be < 1.0001
            expect(res2).to be > 0
            expect(res2).to be < 1.001
        end
        it 'works - single text' do
            text = 'monkey lemur ape ape gear'
            res1 = @client.get_text_similarity('monkeys', text)
            res2 = @client.get_text_similarity('cars', text)
            expect(res1).to be > res2
            expect(res1).to be > 0
            expect(res1).to be < 1.0001
            expect(res2).to be > 0
            expect(res2).to be < 1.001
        end
        it 'works - multi text' do
            text = 'monkey lemur ape ape gear'
            res = @client.multi_get_text_similarity(['monkeys', 'cars'], text)
            scores = res.scores
            expect(scores.keys).to eq(['cars', 'monkeys'])
            res1 = scores['monkeys']
            res2 = scores['cars']
            expect(res1).to be > res2
            expect(res1).to be > 0
            expect(res1).to be < 1.0001
            expect(res2).to be > 0
            expect(res2).to be < 1.001
        end
    end
end
