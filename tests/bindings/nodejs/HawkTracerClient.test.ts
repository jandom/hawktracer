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

    test("receives all events when the events listener is replaced", (done) => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);

        const expected_count = 56;   // number of events in test.htdump
        let firstCount = 0;
        let secondCount = 0;

        const secondCallback = (events) => {
            secondCount += events.length;
            checkEventsCount(firstCount + secondCount, expected_count, done);
        };

        const firstCallback = (events) => {
            if (firstCount == 0) {
                hawkTracerClient.onEvents(secondCallback);
            }
            firstCount += events.length;
            checkEventsCount(firstCount + secondCount, expected_count, done);
        };

        hawkTracerClient.onEvents(firstCallback);
        hawkTracerClient.start();
    });

    test.todo("receives all events if onEvents() is called after start().");
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
    test("correct number of events", (done) => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);

        const expected_count = 56;   // number of events in test.htdump
        let count = 0;
        hawkTracerClient.onEvents((events: object[]) => {
            count += events.length;
            checkEventsCount(count, expected_count, done);
        });
        hawkTracerClient.start();
    });

    function busySleep(seconds) {
        const sleepStart = process.hrtime();
        let sleptSeconds;
        do {
            [sleptSeconds,] = process.hrtime(sleepStart);
        } while (sleptSeconds < seconds);
    }

    test("do not lose events when the callback delays too long", (done) => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);

        const expected_count = 56;   // number of events in test.htdump
        let count = 0;
        let delayed = false;
        hawkTracerClient.onEvents((events: object[]) => {
            count += events.length;
            checkEventsCount(count, expected_count, done);

            if (!delayed) {
                delayed = true;
                // Block the main thread
                busySleep(1);
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
            expect(i).toBe(1);
        });

        hawkTracerClient.start();
    });
});

let countTimer;

function checkEventsCount(count, expected_count, done) {
    expect(count).toBeLessThanOrEqual(expected_count);
    if (count == expected_count) {
        setTimeout(done, 500); // Wait for a while in case more events come in.
    }
    clearTimeout(countTimer);
    countTimer = setTimeout(() => {
        expect(count).toBe(expected_count);
    }, 4000);
}

afterEach(() => {
    clearTimeout(countTimer);
});

