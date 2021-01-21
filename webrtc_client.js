var dataChannel = null;
var peerConnection = null;
var pcConfig = {
  iceServers: [{ url: "stun:stun.l.google.com:19302" }],
};
var iceArray = [];

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
    iceArray.push(evt.candidate);
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

function sdp1() {
  function onOfferSuccess(sessionDescription) {
    peerConnection.setLocalDescription(sessionDescription);
    output("createOffer -> onOfferSuccess");
    output("Offer SDP:begin");
    output(sessionDescription.sdp);
    output("Offer SDP:end");
  }

  function onOfferFailure() {
    output("createOffer -> onOfferFailure");
  }

  peerConnection = new RTCPeerConnection(pcConfig);
  peerConnection.onicecandidate = onIceCandidate;
  peerConnection.ondatachannel = onDataChannel;
  peerConnection.oniceconnectionstatechange = onIceConnectionStateChange;
  var dataChannelOptions = {
    ordered: true, // 順序を保証する
    maxRetransmitTime: 3000, // ミリ秒
  };
  dataChannel = peerConnection.createDataChannel(
    "data_channel",
    dataChannelOptions
  );
  setDataChannelEvents(dataChannel);
  peerConnection.createOffer(onOfferSuccess, onOfferFailure, null);
}

function sdp2() {
  function onAnswerSuccess(sessionDescription) {
    peerConnection.setLocalDescription(sessionDescription);
    output("createAnswer -> onAnswerSuccess");
    output("Answer SDP:begin");
    output(sessionDescription.sdp);
    output("Answer SDP:end");
  }

  function onAnswerFailure() {
    output("createAnswer -> onAnswerFailure");
  }

  var sdp = new RTCSessionDescription({
    type: "offer",
    sdp: input(),
  });
  peerConnection = new RTCPeerConnection(pcConfig);
  peerConnection.onicecandidate = onIceCandidate;
  peerConnection.ondatachannel = onDataChannel;
  peerConnection.oniceconnectionstatechange = onIceConnectionStateChange;
  peerConnection.setRemoteDescription(sdp);
  peerConnection.createAnswer(onAnswerSuccess, onAnswerFailure, null);
}

function sdp3() {
  var sdp = new RTCSessionDescription({
    type: "answer",
    sdp: input(),
  });
  peerConnection.setRemoteDescription(sdp);
}

function ice1() {
  output("ICE:begin");
  output(JSON.stringify(iceArray));
  iceArray = [];
  output("ICE:end");
}

function ice2() {
  var ices = JSON.parse(input());
  for (var ice of ices) {
    var iceObj = new RTCIceCandidate(ice);
    peerConnection.addIceCandidate(iceObj);
  }
}

function send() {
  var data = input();
  dataChannel.send(data);
}

function quit() {
  dataChannel.close();
  dataChannel = null;
  peerConnection.close();
  peerConnection = null;
}
