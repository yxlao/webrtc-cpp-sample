var dataChannel = null;
var peerConnection = null;
var pcConfig = {
  iceServers: [{ url: "stun:stun.l.google.com:19302" }],
};
var iceArray = [];
let webSocketConnection = null;
const webSocketUrl = "ws://localhost:8888";

function output(log) {
  var stdout = document.getElementById("stdout");
  stdout.value = stdout.value + log + "\n";
}

function input() {
  var stdin = document.getElementById("stdin");
  var input = stdin.value;
  stdin.value = "";
  return input;
}

function onDataChannel(evt) {
  output("onDataChannel");
  dataChannel = evt.channel;
  setDataChannelEvents(dataChannel);
}

function onIceCandidate(evt) {
  if (evt.candidate) {
    output("got ice candidate" + evt.candidate);
  } else {
    output("end of ice candidate" + evt.eventPhase);
  }
}

function onIceConnectionStateChange(evt) {
  output("onIceConnectionStateChange:" + peerConnection.iceConnectionState);
}

function setDataChannelEvents(dataChannel) {
  dataChannel.onerror = function (error) {
    ouptput("Data Channel onerror:" + error);
  };

  dataChannel.onmessage = function (event) {
    output("Data Channel onmessage:" + event.data);
    output(String.fromCharCode.apply("", new Uint8Array(event.data)));
  };

  dataChannel.onopen = function () {
    output("Data Channel onopen");
  };

  dataChannel.onclose = function () {
    output("Data Channel onclose");
  };
}

function send() {
  var data = input();
  dataChannel.send(data);
  if (data == "exit") {
    quit();
  }
}

function quit() {
  dataChannel.close();
  dataChannel = null;
  peerConnection.close();
  peerConnection = null;
  output("Client exits gracefully.");
}

function onWebSocketOpen() {
  function onOfferSuccess(sessionDescription) {
    output("[client] onOfferSuccess");
    peerConnection.setLocalDescription(sessionDescription);
    console.log(sessionDescription);
    webSocketConnection.send(JSON.stringify(sessionDescription.toJSON()));
  }

  function onOfferFailure() {
    output("createOffer -> onOfferFailure");
  }

  peerConnection = new RTCPeerConnection(pcConfig);
  peerConnection.onicecandidate = onIceCandidate;
  peerConnection.ondatachannel = onDataChannel;
  peerConnection.oniceconnectionstatechange = onIceConnectionStateChange;
  var dataChannelOptions = {
    ordered: true,
    maxRetransmitTime: 3000,
  };
  dataChannel = peerConnection.createDataChannel(
    "data_channel",
    dataChannelOptions
  );
  setDataChannelEvents(dataChannel);
  peerConnection.createOffer(onOfferSuccess, onOfferFailure, null);
}

function onWebSocketMessage(event) {
  const messageObject = JSON.parse(event.data);
  console.log("[Client] onWebSocketMessage:" + messageObject.type);
  if (messageObject.type == "answer") {
    var sdp = new RTCSessionDescription({
      type: "answer",
      sdp: messageObject.answer, // TODO: rename this to sdp.
    });
    peerConnection.setRemoteDescription(sdp);
  } else if (messageObject.type == "ice") {
    var iceObj = new RTCIceCandidate(messageObject);
    peerConnection.addIceCandidate(iceObj);
  } else {
    throw "Unknown message type.";
  }
}

function connect() {
  webSocketConnection = new WebSocket(webSocketUrl);
  webSocketConnection.onopen = onWebSocketOpen;
  webSocketConnection.onmessage = onWebSocketMessage;
}
