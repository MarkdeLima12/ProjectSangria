## Markdown File for Project Sangria

### Scenario

Etobicoke General Hospital is an understaffed, over capacity hospital in North Etobicoke serving much of Toronto's
lowest income families. Similar to hospitals across Ontario, they have been suffering from staffing burnout, shortages,
and less than a 100% replacement rate. This makes it difficult for medical staff to monitor all their patients at once,
especially as intake numbers continue to rise. Its facility, across many floors and departments, has multiple patients
between its ICUs, CCUs, NICUs, and ERs that need constant monitoring and health checks. Each patient has multiple IoT
monitors enabled, but only some need constant monitoring. The Province has invested money in an AI powered LLM that
will monitor and report on the status of patients, based on data collected from their IoT health monitoring devices.
The system will collect the most recent data from patients, process the data, and send a status update to the medical
team in-charge of that division. The LLM is currently being tested, but it will need a low-level, MQTT interfacing
module that can process the data collected from the sensors and send it to the LLM.

### Problem Statement

Build a critical, resource-constrained C/C++ ingestion component for a high-speed IoT telemetry backend ("Liquid AI
Monitor") that captures health metrics from thousands of sensors and prepares data for a downstream LLM-based anomaly
detection monitor. This component should serve as a "broker" of information, collecting the information from the
various sensors and loading it into a queue.

The core deliverable is the broker core: a high-performance message ring buffer (`FMBuf`) with `push()`/`pop()` style
behavior (implemented here as write/read APIs), designed to:

- sustain very high throughput under typical server workloads,
- minimize memory allocation and overhead,
- favor cache-efficient operations and simple atomic/concurrency-safe patterns,
- avoid heavy external dependencies,
- provide reliable, low-latency ingestion so the Monitor can perform near real-time signal proccessing

Target outcome (benchmark goal): demonstrate sustained ingestion performance toward `>= 1,000,000 messages/second`, with
stable behavior while handling telemetry streams such as status, HR, O2, respiration rate, and temperature.

### Assumptions and Open Questions

#### Protocol and Integration Assumptions

- This module is intended to operate as the ingestion/normalization broker core in an MQTT-based telemetry system.
- Sensor data is expected to enter from MQTT topics, be normalized/queued by this broker core, then be forwarded to
  downstream MQTT subscribers.
- In this repository, MQTT transport calls are represented by stubs and local simulation paths in `main.c`.
- Historical persistence is assumed to be external to this module.

#### Data and Processing Assumptions

- Input payload format in current code is slash-delimited hex: `PatientID/Sensor/Value`.
- One message represents one metric update for one patient at one point in time.
- No input validation/cleaning is performed beyond null checks and hex parsing (`strtol(..., 16)`).
- `processData(...)` mutates the message buffer using `strtok`, so callers must pass writable strings.
- Sensor IDs expected by `addData(...)` are `1..5`.
- Ring buffer capacity is currently unbounded (dynamic linked nodes). The fixed-size check is present but commented
  out for benchmark testing.

#### Runtime and Throughput Assumptions

- Current benchmark run in `main()` generates and processes `1,000,000` messages.
- Measurements are split into `Init`, `Write`, `Read`, and `Print` phases using `clock()`.
- Current implementation is single-threaded; no locks/atomics are used.
- Message ownership is copy-based on both write and read paths (extra allocation/copy overhead is expected).
- `printPatient(...)` is part of measured output flow in current executable.

### System Overview

This project models the core broker stage in a patient-data MQTT pipeline with two main modules:

- `ringBufferAPI` handles message queueing using a ring buffer.
- `patient` handles patient state, field initialization, updates, and display.

Current broker-core code-path data flow:

- `main.c` simulates MQTT ingress by generating synthetic telemetry payloads and writing them to the ring buffer.
- Buffered messages are read and parsed by `processData(...)`.
- Parsed values are mapped to patient fields via `addData(...)`.
- `sendData(...)` and `printPatient(...)` simulate broker egress/reporting after processing.

MQTT broker data flow:

- Sensors publish telemetry to MQTT topics.
- This broker core consumes those messages and places payloads into the ring buffer.
- Buffered data is processed into patient state updates.
- Updated patient state is published by the broker to MQTT for the LLM.
- The LLM publishes status updates back to MQTT, and the broker re-ingests those updates into the patient model.

High-level flow:

1. Initialize a `RingBuffer`.
2. Initialize a `Patient` array (max. 256 entries).
3. Write incoming sensor messages into the buffer.
4. Read/process buffered messages and update patient fields.
5. Publish/send patient data for reporting.

### Data Model

#### `patient` Module

<table>
  <tr>
    <td valign="top">
      <strong>Variables / Struct Fields</strong>
      <ul>
        <li><code>Patient.age</code></li>
        <li><code>Patient.status</code></li>
        <li><code>Patient.HR</code></li>
        <li><code>Patient.O2</code></li>
        <li><code>Patient.respRate</code></li>
        <li><code>Patient.temp</code></li>
      </ul>
    </td>
    <td valign="top">
      <strong>Functions</strong>
      <ul>
        <li><code>initPatient(Patient *arr)</code></li>
        <li><code>processData(Patient *arr, char *message)</code></li>
        <li><code>addData(Patient *patient, SensorType type, void *data)</code></li>
        <li><code>sendData(int patientID)</code></li>
        <li><code>printPatient(Patient *patient)</code></li>
      </ul>
    </td>
  </tr>
</table>

Functionality:

- `initPatient(...)` initializes 256 patient entries.
- `age` is initialized randomly in `[0,100]`, `status` is set to `STATUS_UNKNOWN`, and other sensor fields default to
  `255`.
- `processData(...)` parses `PatientID/Sensor/Value` (hex-delimited) and routes updates through `addData(...)`.
- `addData(...)` maps sensor IDs to patient fields through a `switch(type)`.
- `printPatient(...)` prints readable patient data and displays `UNKNOWN` for fields left at `255`.

Current `PatientStatus` enum numeric values:

- `STATUS_GOOD = 0`
- `STATUS_STABLE = 1`
- `STATUS_FAIR = 2`
- `STATUS_SERIOUS = 3`
- `STATUS_CRITICAL = 4`
- `STATUS_UNKNOWN = 5`
- `STATUS_DECEASED = 6`

#### `ringBufferAPI` Module

<table>
  <tr>
    <td valign="top">
      <strong>Variables / Struct Fields</strong>
      <ul>
        <li><code>BufferNode.message</code></li>
        <li><code>BufferNode.next</code></li>
        <li><code>RingBuffer.read</code></li>
        <li><code>RingBuffer.write</code></li>
        <li><code>RingBuffer.bufferSize</code></li>
      </ul>
    </td>
    <td valign="top">
      <strong>Functions</strong>
      <ul>
        <li><code>initBuffer()</code></li>
        <li><code>bufferWrite(RingBuffer *ringBuffer, char *message)</code></li>
        <li><code>bufferRead(RingBuffer *ringBuffer)</code></li>
        <li><code>freeBufferNode(BufferNode *node)</code></li>
        <li><code>freeRingBuffer(RingBuffer **buffer)</code></li>
      </ul>
    </td>
  </tr>
</table>

Functionality:

- `initBuffer()` allocates and initializes an empty ring buffer.
- `bufferWrite(...)` appends a new node with a copied message.
- `bufferRead(...)` returns the oldest message, advances the read pointer, and frees the old node.
- `freeRingBuffer(...)` drains all remaining messages and frees the buffer safely.
- Current implementation uses a circular linked-node structure with dynamic growth (fixed-capacity check is currently
  commented out).

### Sensor Type Mapping

`SensorType` values are mapped as:

- `STYPE_STATUS = 1`
- `STYPE_HR = 2`
- `STYPE_O2 = 3`
- `STYPE_RESP_RATE = 4`
- `STYPE_TEMPERATURE = 5`

`addData(Patient *patient, SensorType type, void *data)` updates the corresponding `Patient` field via a `switch(type)`
statement.

### Ring Buffer Behaviour

Core operations:

- `initBuffer()` allocates and initializes an empty ring buffer.
- `bufferWrite(RingBuffer *ringBuffer, char* msg)` appends a message node.
- `bufferRead(RingBuffer *ringBuffer)` returns the oldest message and advances the read pointer.
- `freeRingBuffer(RingBuffer **ringBuffer)` drains and frees all buffer memory.
- Each function processes data clean up to ensure that memory is not leaked. Most memory clean up happens in
  bufferRead(...) using helper functions

Design intent:

- FIFO message processing to ensure all data is processed and reported
- Linked-node circular structure with dynamic growth in current code path (capacity check is commented out)
- Explicit memory ownership and cleanup

### MQTT Broker Notes

Current intended MQTT behavior for this project:

- The codebase represents the broker core logic (ingest queueing + parse + state update + egress stub).
- MQTT retained-message behavior and QoS settings are deployment-level concerns for the surrounding transport layer.
- Note: current C code simulates ingress/egress in-process and does not include an MQTT client library.

### Connected Devices

Devices connected to the system that publish and subscribe to data:

<table>
  <tr>
    <td valign="top">
      <strong>Publishing Devices</strong>
      <ul>
        <li>LLM Module (1)</li>
        <li>Heart Rate Monitor (256)</li>
        <li>O2 Monitor (256)</li>
        <li>Respiration Rate Monitor (256)</li>
        <li>Temperature Monitor (256)</li>
      </ul>
    </td>
    <td valign="top">
      <strong>Subscribed Devices</strong>
      <ul>
        <li>LLM Module (1)</li>
        <li>Database (Historical Data) (1)</li>
        <li>Web Dashboard (1 collection point)</li>
      </ul>
    </td>
  </tr>
</table>

#### Information Flow

![MQTT Subscriber/Publisher Chart](./Project_Sangria_MQTT_Data_flow.png)

### Print Behavior

`printPatient(Patient *patient)` prints a formatted patient snapshot.

- Fields equal to `255` are displayed as `UNKNOWN` as this is the default value.
- Non-unknown fields print their numeric value.

### Current Broker Execution Pattern

Typical usage in `main`:

1. Allocate ring buffer.
2. Allocate/declare `Patient patient[256]` storage array.
3. Call `initPatient(patient)` to reset the patient array indices.
4. Simulate ingress with `bufferWrite(...)`.
5. Process records (`bufferRead(...)` + parse + `addData(...)`).
6. Simulate egress/reporting with `sendData(i)` and `printPatient(&patient[i])`.
7. Free resources alloc'd in main.

### Benchmark Testing

Fill in the values below after running your tests.

#### Test Setup

| Item                       | Value                                                                                                     |
|----------------------------|-----------------------------------------------------------------------------------------------------------|
| Build mode                 | Optimized build from Makefile (`-O2`)                                                                     |
| Machine/CPU                | x86_64 server/workstation class CPU (AMD Ryzen 7000 Series)                                               |
| Compiler + flags           | `gcc -std=c11 -Wall -Wextra -O2`                                                                          |
| Include path               | `-Iheaders`                                                                                               |
| Build target               | `main`                                                                                                    |
| Build command              | `make`                                                                                                    |
| Run command                | `make run`                                                                                                |
| Dataset size (messages)    | `1,000,000` synthetic telemetry messages                                                                  |
| Number of patients touched | up to `256` (`Patient patient[256]`)                                                                      |
| Message format under test  | `PatientID/Sensor/Value` (hex-delimited string payload)                                                   |
| Sensor distribution        | randomized across sensor IDs `1..5` during message generation                                             |
| Timing scope               | separate timings for `Init`, `Write`, `Process (Read)`, and `Print`                                       |
| Benchmark flow             | pre-populate/write buffer, then read+process, then print all patient records                              |
| Notes                      | current timing uses `clock()` and includes string parsing + memory allocation behavior in measured phases |

#### Results Table

5 runs were completed to adequately minimize the affects of anomolies in the code. Additionally, as messages are
randomized, the increase in runs will help normalize the data, giving an accurate representation of each stage.
Please note that each run was done with 1 million, randomly generate messages. Before each run, the compiled objects
were cleared out, and the code was recompiled freshly. This was to maintain consistency with each run and remove any
change in results.

| Operation      | Total Time (secs) | Runs |    Average |        Min |        Max | Notes                                                                       |
|----------------|------------------:|-----:|-----------:|-----------:|-----------:|-----------------------------------------------------------------------------|
| Init           |         0.00024 s |    5 | 0.000047 s | 0.000029 s | 0.000058 s | Fairly consistent per run. init time will increase with scaling.            |
| Write          |         31.7307 s |    5 |  6.34615 s |  5.99362 s |  6.85074 s | Consistent per run. Scaling won't affect per message write time.            |
| Process (Read) |         0.52012 s |    5 |  0.10402 s |  0.13542 s |  0.13542 s | Consistent per run. Scaling won't affect per message read and process time. |
| Print          |         0.16039 s |    5 |  0.03208 s |  0.03693 s |  0.03693 s | Consistent per run. Scaling in this area will increase with scaling.        |

#### Benchmark Notes

- Timing method used: `clock()` (CPU time based)
- Warm-up performed: No explicit warm-up in current implementation
- I/O included in timing (`yes/no`): Yes for `Print` phase (stdout output)
- Any bottlenecks observed: Frequent allocation/copy/free in ring buffer message handling and console I/O during
  printing are expected hotspots

### Scaling

#### Sensor Collection

When we think about scaling up sensor collection to fit the scale of a large hospital (500+ beds), there will be
collisions and potential processing issues. Here are some ways to help alleviate those issues:

- Setting up smaller that smaller groups of sensors feed into. The main buffer that connects to the broker then reads
  one from each buffer, in order, then repeats.
- Creating custom brokers that handle smaller sets, but more sensors including specialized sensors.

#### Processing Scaling

When we scale up sensor collection, and maybe specialize it, this may lead to more processing requirements. Here are
some suggestions to alleviate that.

- One large "main buffer" that has smaller buffers within it, responsible for specialized sensor collection. Two brokers
  that do specialized processing and select nodes from specific browsers.
- One main buffer attached to a load balancer that distributes messages between a cluster brokers that process the data
  as they are handed them. This can be scaled up and down as needed.
