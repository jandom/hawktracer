const {HawkTracerClient} = require('@hawktracer/client')

const source = require('path').join(__dirname, 'test.htdump');
const hawkTracerClient = new HawkTracerClient(source);

hawkTracerClient.onEvents((events) => console.log(events));

hawkTracerClient.start();

setTimeout(() => hawkTracerClient.stop(), 1000);
