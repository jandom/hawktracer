const HawkTracerClient = require('@hawktracer/client')
const source = require('path').join(__dirname, 'test.htdump');
hawkTracerClient = new HawkTracerClient(source);
hawkTracerClient.start();
hawkTracerClient.end();
