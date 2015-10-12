Gem::Specification.new do |s|
    s.name        = 'relevanced_client'
    s.version     = '0.9.0rc2'
    s.date        = '2015-10-11'
    s.summary     = "relevanced client"
    s.description = "Ruby client for relevanced-server"
    s.authors     = ["Scott Ivey"]
    s.email       = 'scott.ivey@gmail.com'
    s.files       = [
        "lib/relevanced_client.rb",
        "lib/gen-rb/relevanced.rb",
        "lib/gen-rb/relevanced_protocol_constants.rb",
        "lib/gen-rb/relevanced_protocol_types.rb"
    ]
    s.require_paths = ['lib']
    s.homepage    = 'http://www.relevanced.org'
    s.license       = 'MIT'

    s.add_runtime_dependency 'thrift', '~> 0.9.3.0', '>= 0.9.3.0'
end
