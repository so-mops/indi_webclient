// Logging class for this module
// Use it by typing logging.warning('this is the message');
// Just like python!
// TODO add build number to prevent caching
const loggingFormat = [
  { // debug
    "color": "text-muted",
    "label": "DEBUG"
  },
  { // info
    "color": "text-black",
    "label": "INFO"
  },
  { // warning
    "color": "text-warning",
    "label": "WARNING"
  },
  { // danger
    "color": "text-danger",
    "label": "ERROR",
  },
  { //critical
    "color": "bg-danger",
    "label": "CRITICAL"
  }
]

const logMessage = (message, level) => {
  //https://stackoverflow.com/questions/18614301/keep-overflow-div-scrolled-to-bottom-unless-user-scrolls-up

  var logText = document.getElementById("guimsg");

  var newMessage = document.createElement("p");
  var color = loggingFormat[level].color;
  var label = loggingFormat[level].label;
  var timestamp = new Date().toISOString();
  newMessage.classList.add(color, "mb-0")

  var logHeight = logText.scrollHeight;
  var isScrolledToBottom = logText.scrollHeight - logText.clientHeight <= logText.scrollTop + 1;


  // https://stackoverflow.com/questions/25505778/automatically-scroll-down-chat-div
  newMessage.textContent = `${timestamp} - ${label} - ${message}`
  logText.appendChild(newMessage);
  if (isScrolledToBottom) {
    logText.scrollTo(0, logHeight,);
  }

}
const logging = {
  async debug(message) {
    if (document.getElementById("debugCheckbox").checked) {
      logMessage(message, 0);
    }
  },
  async info(message) {
    if (document.getElementById("infoCheckbox").checked) {
      logMessage(message, 1);
    }
  },
  async warning(message) {
    if (document.getElementById("warningCheckbox").checked) {
      logMessage(message, 2);
    }
  },
  async error(message) {
    if (document.getElementById("errorCheckbox").checked) {
      logMessage(message, 3);
    }

  },
  async critical(message) {
    if (document.getElementById("criticalCheckbox").checked) {
      logMessage(message, 4);
    }
  }
}
