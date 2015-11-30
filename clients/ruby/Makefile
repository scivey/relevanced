.PHONY: test publish

test:
	cd client && rspec

publish:
	cd client && npm publish
	rm -f client/*.gem
	cd client && gem build relevanced_client.gemspec
	mv client/*.gem client/relevanced_client.gem
	gem push client/relevanced_client.gem
