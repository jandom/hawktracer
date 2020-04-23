import {HawkTracerClient} from "../../../index";

test("Create Instance", () => {
    new HawkTracerClient({source: "abc"});
})