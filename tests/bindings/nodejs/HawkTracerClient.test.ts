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

describe("2. Set up callback", () => {
    test("invokes callback", (done) => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);

        hawkTracerClient.onEvents(() => {
            done();
        });
        hawkTracerClient.start();
    });
});

describe("3. Start HawkTracerClient", () => {
    test("succeeds with existing source file", () => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);
        expect(hawkTracerClient.start()).toBe(true);
    });

    test.todo("succeeds with source in 'x.x.x.x:p' format where x.x.x.x is the IP address, p is port number");

    test("fails with non-existing source file", () => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const hawkTracerClient = new HawkTracerClient('non-existing file !@£$%^&*()');
        expect(hawkTracerClient.start()).toBe(false);
    });
});

describe("4. Receive events through callback", () => {
    const expected_count = 56;   // number of events in test.htdump
    let count;
    let timer;
    beforeEach(() => {
        jest.setTimeout(1000000);   // We'll have custom timer.
        timer = setTimeout(() => {
            throw new Error("Too few events, expected: " + expected_count + ", actual: " + count);
        }, 5000);
        count = 0;
    });

    afterEach(() => {
        timer.unref();
    });

    test("correct number of events", (done) => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);

        hawkTracerClient.onEvents((events: object[]) => {
            count += events.length;
            if (count > expected_count) {
                throw new Error("Too many events, expected: " + expected_count + ", actual: " + count);
            }
            if (count == expected_count) {
                setTimeout(done, 500); // Wait for a while in case more events come in.
            }
        });
        hawkTracerClient.start();
    });

    test("do not lose events when the callback delays too long", (done) => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);

        let delayed = false;
        hawkTracerClient.onEvents((events: object[]) => {
            count += events.length;
            if (count > expected_count) {
                throw new Error("Too many events, expected: " + expected_count + ", actual: " + count);
            }
            if (count == expected_count) {
                expect(delayed).toBe(true);
                setTimeout(done, 500); // Wait for a while in case more events come in.
            }
            if (!delayed) {
                delayed = true;
                // Block the main thread
                const delayStart = process.hrtime();
                let delayedSeconds;
                do {
                    [delayedSeconds,] = process.hrtime(delayStart);
                } while (delayedSeconds < 1);    // delays 1 second
            }
        });
        hawkTracerClient.start();
    });
});

describe("5. Stop HawkTracerClient", () => {
    test("succeeds and events stop coming", (done) => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);

        let i = 0;
        hawkTracerClient.onEvents(() => {
            ++i;
            if (i == 1) {
                // noinspection JSDeprecatedSymbols
                hawkTracerClient.stop();
                setTimeout(done, 500);
                return;
            }
            throw new Error("Events were received after stop.");
        });

        hawkTracerClient.start();
    });
});
