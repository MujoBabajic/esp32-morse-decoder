import { initializeApp } from "https://www.gstatic.com/firebasejs/11.2.0/firebase-app.js";
import {
  getDatabase,
  ref,
  set,
} from "https://www.gstatic.com/firebasejs/11.2.0/firebase-database.js";
import {firebaseConfig} from "./firebase_config.js";

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const db = getDatabase();

function insertData() {
  // Creating a timestamp for the message
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

  // Sending the timestamp and message to Firebase
  set(ref(db, "MessageLog" + timestamp), {
    timestamp: timestamp,
    message: textInput.value,
  })
    .then(() => {
      alert("Data sent successfully");
    })
    .catch((error) => {
      console.log(error);
      alert("Error: ", error);
    });
}

sendBtn.addEventListener("click", insertData);
