test("return hello object", () => {
    const hello_provider = require('bindings')('hawktracer_client');   // loads build/Release/hawktracer_client.node
    expect(hello_provider).not.toBeNull()

    const hello = hello_provider.hello
    expect(hello).not.toBeNull()
    expect(hello.hello).toBe('world')
    expect(hello[42]).toBe('The Answer to Life, the Universe, and Everything')
    expect(hello["Douglas Adams"]).toBe(true)
})