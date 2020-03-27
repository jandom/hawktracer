describe("1. Initiating HawkTracerClient", () => {
    test("succeeds when constructor is given `source` parameter in string type", () => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');   // loads build/Release/hawk_tracer_client.node
        const hawkTracerClient = new HawkTracerClient("some-file");
        expect(hawkTracerClient).not.toBeNull();
    });

    test("fails without parameter", () => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        expect(() => {
            new HawkTracerClient();
        }).toThrow();
    });

    test("fails with parameter in wrong type", () => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        expect(() => {
            new HawkTracerClient(11);
        }).toThrow();
    });
});

describe("2. Set up data callback", () => {
    const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
    const source = require('path').join(__dirname, 'test.htdump');
    const hawkTracerClient = new HawkTracerClient(source);

    test("invokes data callback", (done) => {
        hawkTracerClient.onEvents(() => {
            done();
        });
        hawkTracerClient.start();
    });
});

describe("3. Start HawkTracerClient", () => {
    const {HawkTracerClient} = require('bindings')('hawk_tracer_client');

    test("succeeds with existing source file", () => {
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);
        expect(hawkTracerClient.start()).toBe(true);
    });

    test.todo("succeeds with source in 'x.x.x.x:p' format where x.x.x.x is the IP address, p is port number");

    test("fails with non-existing source file", () => {
        const hawkTracerClient = new HawkTracerClient('non-existing file !@£$%^&*()');
        expect(hawkTracerClient.start()).toBe(false);
    });
});

describe("4. Receive data through callback", () => {
    const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
    const source = require('path').join(__dirname, 'test.htdump');
    const hawkTracerClient = new HawkTracerClient(source);

    test("correct number of data", (done) => {
        let count = 0;
        hawkTracerClient.onEvents((events: object[]) => {
            console.log(JSON.stringify(events));
            count += events.length;
            if (count > 56) {   // number of events in test.htdump
                throw new Error("Too many data");
            }
            if (count == 56) {
                setTimeout(done, 1000);
            }
        });
        hawkTracerClient.start();
    });
});

