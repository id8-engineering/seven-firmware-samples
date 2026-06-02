# Tutorial for Mender.io and AWS IoT Core sample

This sample combines the AWS IoT and Mender integrations on the Seven nRF9151
target. After boot, the device brings up LTE, initializes the Mender client,
connects to AWS IoT over MQTT, and then publishes a small JSON message to the
configured AWS output topic at a fixed interval. Any message received on the
AWS input topic is logged by the device.

The sample demonstrates how the two services can be used together to solve
different parts of the device lifecycle. Mender is used for OTA software
updates and deployment management, while AWS IoT Core is used for device-cloud
connectivity and message exchange over MQTT.

The expected end result is a board that shows up in Mender as an activated
device, connects to AWS IoT with the configured thing name and certificates,
publishes periodic MQTT messages, and logs inbound AWS messages on the serial
console.

## Prerequisites

* See more prerequisites [here](https://github.com/id8-engineering/seven-firmware-samples/blob/main/docs/PREREQUISITES.md).
* Install [mender-artifact](https://docs.mender.io/downloads/workstation-tools#mender-artifact)
* [A Mender account or sign up for a new account](https://eu.hosted.mender.io/ui/signup)
* [An AWS account with access to AWS IoT Core, or sign up for a new account](https://signin.aws.amazon.com/signup?request_type=register)

## Hardware

* [Seven](https://github.com/id8-engineering/seven-hardware)
* Nano SIM card with active data plan
* LTE antenna with SMA contact

## Get started

To run this sample there is a few steps that you need to follow to do first, please follow these steps down below in the same order they are listed.

## Setup AWS Iot Core

### Create your *policy*:

Before you create your policy you will need to find your *AWS account ID*.

1. Go to [IAM](https://us-east-1.console.aws.amazon.com/iam?region=eu-north-1#/home) inside AWS.
2. On the right side of the page locate you *Account ID* and save it for later. For example it could be 082674678567 and is always a 12 number digit value.

Before you create your policy you will need to find your *AWS account region.*

1. Go to AWS IoT Core and look at the start of link to the page you are on. There you will find your region, for example it can be *eu-north-1*.

1. Go to AWS IoT core and under *Manage* press *Security* -> *Policies*
2. Press *Create policy* and give your new policy a name, for example *Seven-nrf9151-policy*.
3. Select *JSON*, remove whats there and copy/paste this:

> [!NOTE]
> Change <PASTE-YOUR-REGION>:<PASTE-YOUR-ACCOUNT-ID> to the your account specific values.

```JSON
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": "arn:aws:iot:<PASTE-YOUR-REGION>:<PASTE-YOUR-ACCOUNT-ID>:client/*"
    },
    {
      "Effect": "Allow",
      "Action": [
        "iot:Publish",
        "iot:Receive",
        "iot:Subscribe"
      ],
      "Resource": [
        "arn:aws:iot:<PASTE-YOUR-REGION>:<PASTE-YOUR-ACCOUNT-ID>:topic/*",
        "arn:aws:iot:<PASTE-YOUR-REGION>:<PASTE-YOUR-ACCOUNT-ID>:topicfilter/*"
      ]
    }
  ]
}
```

### Create your *thing*:

1. Inside AWS portal, access IoT Core service.
2. Under *Manage* press to *Things*.
3. Press *Create things*.
4. Select *Create single thing* and press next.
5. Give you device a name, for example *Seven-nrf9151* then press next.
6. Select *Auto-generate a new certificate* and press next.
7. Select the policy you made in previous step and create thing.
8. Download the files, NOTE that this is the only time you can download the files:
  * Device certificate
  * Private key file
  * Public key file
  * Amazon trust services endpoint (CA 1)

## Write security tags to modem

TLS credentials can be stored securely in the modem instead of inside the firmware image.

### Write certificates with nrfcredstore

Before you can write the certificates to the modem you will have to build and flash the at-sample:

>[!NOTE]
>Seven provides a extra `config` file to be able to talk to the modem over rtt, this custom made sample will not work to run `nrfcredstore`.
>To be able to run `nrfcredstore` you will need to build without this extra `config` file.
>
>If you would like to debug Seven after flashing [Nrf at-client sample](https://github.com/nrfconnect/sdk-nrf/tree/main/samples/cellular/at_client) 
>you will need to connect to Sevens onboard UART pin header.

```bash
west build -p always -b seven/nrf9151/ns ../nrf/samples/cellular/at_client/
west flash
```

Connect the USB-to-UART adapter to the Seven UART pin header:

* Adapter TX to Seven RX
* Adapter RX to Seven TX
* Adapter GND to Seven GND

Install nrfcredstore:

```bash
pip3 install nrfcredstore
```

Put the downloaded AWS IoT credentials in `~/Downloads` with these names:

* `AmazonRootCA1.pem`
* `device-certificate.pem.crt`
* `private-key.pem.key`

Write the credentials to modem security tag `123`, or any other number:

```bash
nrfcredstore --cmd-type at --rtscts-off /dev/ttyUSB0 write 123 ROOT_CA_CERT ~/Downloads/AmazonRootCA1.pem
nrfcredstore --cmd-type at --rtscts-off /dev/ttyUSB0 write 123 CLIENT_CERT ~/Downloads/device-certificate.pem.crt
nrfcredstore --cmd-type at --rtscts-off /dev/ttyUSB0 write 123 CLIENT_KEY ~/Downloads/private-key.pem.key
```

### Write Mender HTTPS CA certificates

The nRF modem needs the Hosted Mender HTTPS certificate chain in its credential
store. Fetch the chain and split it into PEM files:

```bash
cd ~/Downloads

openssl s_client \
    -showcerts \
    -connect eu.hosted.mender.io:443 \
    -servername eu.hosted.mender.io </dev/null \
    2>/dev/null | awk '
        /BEGIN CERTIFICATE/ { n++ }
        n { print > ("mender-cert-" n ".pem") }
        /END CERTIFICATE/ { close("mender-cert-" n ".pem") }
    '

ls -lh mender-cert-*.pem
```

Write the first two certificates to the Mender security tags used by this
sample:

```bash
nrfcredstore --cmd-type at --rtscts-off /dev/ttyUSB0 write 1 ROOT_CA_CERT ~/Downloads/mender-cert-1.pem
nrfcredstore --cmd-type at --rtscts-off /dev/ttyUSB0 write 2 ROOT_CA_CERT ~/Downloads/mender-cert-2.pem
```

List credentials on modem:

```bash
nrfcredstore --cmd-type at --rtscts-off /dev/ttyUSB0 list
```

The list should include these entries:

```text
1    ROOT_CA_CERT
2    ROOT_CA_CERT
123  ROOT_CA_CERT
123  CLIENT_CERT
123  CLIENT_KEY
```

## Build sample

Build sample, NOTE replace the <PLACEHODLERS> to the right values:

Placeholders:
* Mender tenant token
> [!TIP] Enter https://eu.hosted.mender.io/ui/settings/organization and copy *Organization token* to
> clipboard.
* AWS endpoint 
> [!TIP] Under *Connect* press *Domain configuration* and copy the *Domain name*.
* AWS thing name
* Sec tag

Export the values used by the build:

```bash
export MENDER_TENANT_TOKEN="<paste your token here>"
export AWS_ENDPOINT="<paste your AWS IoT endpoint here>"
export AWS_THING_NAME="<paste your AWS thing name here>"
export SEC_TAG="<paste your modem security tag here>"
```

```bash
west build --sysbuild -p always -b seven/nrf9151/ns samples/aws_and_mender -- \
    "-Daws_and_mender_CONFIG_MENDER_SERVER_TENANT_TOKEN=\"${MENDER_TENANT_TOKEN}\"" \
    "-Daws_and_mender_CONFIG_AWS_IOT_BROKER_HOST_NAME=\"${AWS_ENDPOINT}\"" \
    "-Daws_and_mender_CONFIG_AWS_IOT_CLIENT_ID_STATIC=\"${AWS_THING_NAME}\"" \
    "-Daws_and_mender_CONFIG_MQTT_HELPER_SEC_TAG=${SEC_TAG}" \
    "-Dmcuboot_EXTRA_CONF_FILE=$(pwd)/samples/aws_and_mender/sysbuild/mcuboot.conf"
```

## Flash sample

1. Flash sample:

```bash
west flash
```

2. Monitor sample:

```bash
west rtt
```

3. After accessing console for the first time your log will look like this:

```text
SEGGER J-Link V9.18 - Real time terminal output
SEGGER J-Link V13.0, SN=853001439
Process: JLinkGDBServerCLExe
*** Booting nRF Connect SDK v3.2.1-d8887f6f32df ***
*** Using Zephyr OS v4.2.99-ec78104f1569 ***
[1970-01-01T00:00:00,256683Z] <inf> main: Starting LTE
[1970-01-01T00:00:05,633514Z] <inf> main: LTE connected
[1970-01-01T00:00:05,633575Z] <inf> mender: Device type: [seven_nrf9151]
[1970-01-01T00:00:05,640258Z] <inf> main: Mender activated
[1970-01-01T00:00:05,640289Z] <inf> main: Starting AWS
[1970-01-01T00:00:05,640869Z] <inf> main: Connecting to AWS IoT
[1970-01-01T00:00:05,641723Z] <inf> mender: Initialization done
[1970-01-01T00:00:05,641784Z] <inf> mender: Checking for deployment...
[1970-01-01T00:00:09,757995Z] <inf> main: AWS_IOT_EVT_CONNECTED
[1970-01-01T00:00:09,758941Z] <inf> main: Published "{"message":"Hello from seven_nrf9151!"}" to "seven_nrf9151/output"
[1970-01-01T00:00:09,818145Z] <inf> main: AWS_IOT_EVT_PUBACK, message ID: 55133
[1970-01-01T00:00:10,110168Z] <err> mender: [401] Unauthorized: dev auth: unauthorized
[1970-01-01T00:00:10,110565Z] <err> mender: Authentication failed
[1970-01-01T00:00:10,110595Z] <err> mender: Unable to perform HTTP request
[1970-01-01T00:00:10,110809Z] <err> mender: Unknown error occurred, status=0
[1970-01-01T00:00:10,110839Z] <err> mender: Unable to check for deployment
[1970-01-01T00:00:10,110900Z] <err> mender: Work mender_client_main failed, retrying in 60 seconds
```

Follow the [Accept device](#accept-device) to Authorize device.

Once device is accepted log console should look like this:

```text
SEGGER J-Link V9.18 - Real time terminal output
SEGGER J-Link V13.0, SN=853001439
Process: JLinkGDBServerCLExe
*** Booting nRF Connect SDK v3.2.1-d8887f6f32df ***
*** Using Zephyr OS v4.2.99-ec78104f1569 ***
[1970-01-01T00:00:00,256683Z] <inf> main: Starting LTE
[1970-01-01T00:00:32,750305Z] <inf> main: LTE connected
[1970-01-01T00:00:32,750366Z] <inf> mender: Device type: [seven_nrf9151]
[1970-01-01T00:00:32,757049Z] <inf> main: Mender activated
[1970-01-01T00:00:32,757049Z] <inf> main: Starting AWS
[1970-01-01T00:00:32,757659Z] <inf> main: Connecting to AWS IoT
[1970-01-01T00:00:32,758514Z] <inf> mender: Initialization done
[1970-01-01T00:00:32,758544Z] <inf> mender: Checking for deployment...
[1970-01-01T00:00:36,709899Z] <inf> main: AWS_IOT_EVT_CONNECTED
[1970-01-01T00:00:36,710845Z] <inf> main: Published "{"message":"Hello from seven_nrf9151!"}" to "seven_nrf9151/output"
[1970-01-01T00:00:36,775817Z] <inf> main: AWS_IOT_EVT_PUBACK, message ID: 20915
[1970-01-01T00:00:38,527648Z] <inf> mender: No deployment available
```

## Mender deploy artifact

If you would like to update your device OTA(Over The Air), follow these steps:

1. Enter Mender https://eu.hosted.mender.io/ui/
2. Go to *Software* and press *Upload an artifact*
3. Upload zephyr.mender artifact, it can be found from the repo root where you built the sample under build/aws_and_mender/zephyr/
4. Go to *Deployments* and *Create a deployment*
5. Select these settings:
  * *Select a device group to target*: All devices
  * *Select a Release to deploy*: V0.1
  * *Select a start time*: Start immediately
  * *Show advanced optins*: Select *Force update*
6. Create deployment

Example logs of successful deployment:

If deployment is found:

```
[1970-01-01T00:01:40,221252Z] <inf> mender: Downloading artifact with id '48ba800...', name 'V0.1', uri 'https://c271964d41749feb10da762816c952ee.r2.cloudfling
```

Artifact is installing:

```
[1970-01-01T00:02:46,052459Z] <inf> mender: Downloading 'zephyr-image' 10%... [43008/426910]
[1970-01-01T00:03:39,484863Z] <inf> mender: Downloading 'zephyr-image' 20%... [85504/426910]
```

Artifact is installed, swap completed and rollback cancelled:

```
[1970-01-01T00:10:55,508239Z] <inf> mender: Download done, installing artifact
[1970-01-01T00:10:56,998809Z] <inf> main: Mender deployment status: installing
[1970-01-01T00:10:57,002685Z] <inf> mender: Artifact installation done, rebooting
[1970-01-01T00:10:58,435882Z] <inf> main: Mender deployment status: rebooting
[1970-01-01T00:10:58,439453Z] <wrn> main: Mender requested reboot; waiting 5s for logs
[1970-01-01T00:10:59,834960Z] <inf> main: Published "{"message":"Hello from seven_nrf9151!"}" to "seven_nrf9151/output"
*** Booting nRF Connect SDK v3.2.1-d8887f6f32df ***
*** Using Zephyr OS v4.2.99-ec78104f1569 ***
[1970-01-01T00:00:00,256744Z] <inf> main: Starting LTE
[1970-01-01T00:00:44,116729Z] <inf> main: LTE connected
[1970-01-01T00:00:44,116790Z] <inf> mender: Device type: [seven_nrf9151]
[1970-01-01T00:00:44,122833Z] <inf> main: Mender activated
[1970-01-01T00:00:44,122863Z] <inf> main: Starting AWS
[1970-01-01T00:00:44,123443Z] <inf> main: Connecting to AWS IoT
[1970-01-01T00:00:44,125122Z] <inf> mender: Initialization done
[1970-01-01T00:00:49,193725Z] <inf> main: AWS_IOT_EVT_CONNECTED
[1970-01-01T00:00:49,194641Z] <inf> main: Published "{"message":"Hello from seven_nrf9151!"}" to "seven_nrf9151/output"
[1970-01-01T00:00:49,248962Z] <inf> main: AWS_IOT_EVT_PUBACK, message ID: 25900
[1970-01-01T00:00:51,974822Z] <inf> main: Mender deployment status: installing
[1970-01-01T00:00:51,975463Z] <inf> mender: Application has been mark valid and rollback canceled
[1970-01-01T00:00:53,985900Z] <inf> main: Mender deployment status: success
[1970-01-01T00:00:59,202026Z] <inf> main: Published "{"message":"Hello from seven_nrf9151!"}" to "seven_nrf9151/output"
[1970-01-01T00:01:09,209442Z] <inf> main: Published "{"message":"Hello from seven_nrf9151!"}" to "seven_nrf9151/output"
[1970-01-01T00:01:19,216857Z] <inf> main: Published "{"message":"Hello from seven_nrf9151!"}" to "seven_nrf9151/output"
[1970-01-01T00:01:29,218383Z] <inf> main: Published "{"message":"Hello from seven_nrf9151!"}" to "seven_nrf9151/output"
[1970-01-01T00:01:39,225799Z] <inf> main: Published "{"message":"Hello from seven_nrf9151!"}" to "seven_nrf9151/output"
[1970-01-01T00:01:49,233184Z] <inf> main: Published "{"message":"Hello from seven_nrf9151!"}" to "seven_nrf9151/output"
[1970-01-01T00:01:53,993530Z] <inf> mender: Checking for deployment...
[1970-01-01T00:01:55,534118Z] <inf> mender: No deployment available
```

Artifact download and swap will start when device checks for deployment. Note that this artifact is the same artifact that you are already running so it will not change anything. Feel free to update something in the code before then build again and upload the artifact and you will see the new app running after image swap is done.

## Accept device

First time a device tries to connect to Mender it needs to be accepted from Mender, you will find *Pending Devices* inside *Devices* where you can accept your device if everything is done correctly.

## AWS read/send messages

1. Enter AWS IoT Core
2. Under *Test* open *MQTT test client*
3. Under *Subscribe to a topic* enter in *Topic filer* bar <YOUR_THING_NAME>/input & <YOUR_THING_NAME>/output
4. Messages from your device will appear under /output and message you want to send to the device can be sent from /input.

Example log of working end to end communication from device to AWS:

```text
[1970-01-01T00:03:19,886352Z] <inf> main: Published "{"message":"Hello from seven_nrf9151!"}" to "seven_nrf9151/output"
[1970-01-01T00:03:29,649536Z] <inf> main: Received AWS message: "{
  "message": "Hello from AWS IoT console"
}" on topic "seven_nrf9151/input"
[1970-01-01T00:03:29,887878Z] <inf> main: Published "{"message":"Hello from seven_nrf9151!"}" to "seven_nrf9151/output"
```
