test("Create HawkTracerClient instance", () => {
    const {HawkTracerClient} = require('bindings')('hawk_tracer_client');   // loads build/Release/hawktracer_client.node
    const hawkTracerClient = new HawkTracerClient();
    expect(hawkTracerClient.start()).toBe(true);
});
