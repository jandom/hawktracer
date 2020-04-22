const NativeClient = require('bindings')('hawk_tracer_client').HawkTracerClient;   // loads build/Release/hawk_tracer_client.node

export interface HawkTracerClientOptions {
    source: string;
}

export interface Event {
    timestamp: number;
    id: number;
}

export interface EventKlassInfoEvent extends Event {
    info_klass_id: number;
    event_klass_name: string;
    field_count: number;
}

export interface EventKlassFieldInfoEvent extends Event {
    info_klass_id: number;
    field_type: string;
    field_name: string;
    size: number;
    data_type: number;
}

export interface CallstackEvent extends Event {
    duration: number;
    thread_id: number;
    label?: string | number;
}

export interface StringMapping extends Event {
    identifier: number;
    label: string;
}

export interface SystemInfoEvent extends Event {
    version_major: number;
    version_minor: number;
    version_patch: number;
}

export class HawkTracerClient {
    private _client: any;

    constructor(options: HawkTracerClientOptions) {
        this._client = new NativeClient(options.source);
    }

    public start(): boolean {
        return this._client.start();
    }

    public stop(): void {
        this._client.stop();
    }

    public onEvents(callback: (events: Event[]) => void): void {
        this._client.onEvents(callback);
    }
}
