# Javascript Client

---

See the [detailed Javascript API documentation here.](../api/javascript_api.md)

---

## Installation

The Javascript client for NodeJS is available on [NPM](https://www.npmjs.com/package/relevanced-client); you can install it in an NPM-managed project by running:

```bash
npm install relevanced-client --save
```

## Basic Use

```javascript
var RelevancedClient = require('relevanced-client').RelevancedClient;
var client = new RelevancedClient('localhost', 8097);
client.connect().then(function(client) {
    client.listAllDocuments().then(function(documents) {
        console.log("Document count: " + documents.length);
        client.disconnect();
    });
});
```

The `RelevancedClient` constructor also has a promise-returning static factory method which creates a new client and calls `connect()` on it:
```javascript
var RelevancedClient = require('relevanced-client').RelevancedClient;
RelevancedClient.connect('localhost', 8097).then(function(client) {
    client.listAllDocuments().then(function(documents) {
        console.log("Document count: " + documents.length);
        client.disconnect();
    });
});
```

## Examples
* [Javascript binary classifier](../examples/javascript-binary-classifier)