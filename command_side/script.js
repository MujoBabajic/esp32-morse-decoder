import { initializeApp } from "https://www.gstatic.com/firebasejs/11.2.0/firebase-app.js";
import {
  getDatabase,
  ref,
  set,
} from "https://www.gstatic.com/firebasejs/11.2.0/firebase-database.js";
import {firebaseConfig} from "./firebase_config.js";

const textInput = document.getElementById("textInput");
const sendBtn = document.getElementById("sendBtn");
const validationText = document.getElementById("validationText");

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const db = getDatabase();

function validateInput(input){
  // Regex for input validation
  const validationPattern = /^[a-zA-Z0-9 ]*$/;

  if (!input.value || !validationPattern.test(input.value)) {
    validationText.innerText = !input.value 
      ? "No text entered" 
      : "Enter only alphanumeric characters";
      
    validationText.style.visibility = "visible";
    return false;
  }

  if (input.value.length > 50) {
    validationText.innerText = `Message too long (Max 50 characters)`;
    validationText.style.visibility = "visible";
    return false;
  }

  return true;
}

// Creating a timestamp for the message
function createTimestamp(){
  const date = new Date();
  const timestamp = `${date.getFullYear()}${String(date.getMonth() + 1).padStart(
    2,
    "0"
  )}${String(date.getDate()).padStart(2, "0")}${String(date.getHours()).padStart(
    2,
    "0"
  )}${String(date.getMinutes()).padStart(2, "0")}${String(
    date.getSeconds()
  ).padStart(2, "0")}`;

  return timestamp;
}

function insertData() {
  textInput.value=textInput.value.trim();

  if(!validateInput(textInput)){
    return;
  }

  const timestamp = createTimestamp();

  // Sending the timestamp and message to Firebase
  set(ref(db, "MessageLog" + timestamp), {
    timestamp: timestamp,
    message: textInput.value,
  })
    .then(() => {
      validationText.style.visibility = "hidden";
      alert("Data sent successfully");
    })
    .catch((error) => {
      console.log(error);
      alert("Error: ", error);
    });
}

sendBtn.addEventListener("click", insertData);
