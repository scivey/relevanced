lint:
	pyflakes client/relevanced_client/client.py

test:
	cd client && nosetests relevanced_client/test

publish:
	cd client && python setup.py register -r pypi && python setup.py sdist upload -r pypi

.PHONY: lint test publish
