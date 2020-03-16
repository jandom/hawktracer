var addon = require('bindings')('hawktracer_client');   // loads build/Release/hawktracer_client.node

console.log(JSON.stringify(addon.hello));