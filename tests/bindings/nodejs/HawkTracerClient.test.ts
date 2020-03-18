test("Create HawkTracerClient instance", () => {
    const {HawkTracerClient} = require('bindings')('hawk_tracer_client');   // loads build/Release/hawktracer_client.node
    const hawkTracerClient = new HawkTracerClient("some-file");
    expect(hawkTracerClient).not.toBeNull();
});

test("Create HawkTracerClient instance without source", () => {
    const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
    expect(() => {
        new HawkTracerClient();
    }).toThrow();
});

test("Create HawkTracerClient instance with invalid source", () => {
    const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
    expect(() => {
        new HawkTracerClient(11);
    }).toThrow();
});
