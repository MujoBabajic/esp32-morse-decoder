const esp32IP = "192.168.5.1";

document.getElementById("sendBtn").addEventListener("click", sendMessage);

function sendMessage() {
    const message = document.getElementById('textInput').value;
    if (!message) {
      alert('Please enter a message.');
      return;
    }

    fetch(`http://${esp32IP}/receive`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ message: message }),
    })
      .then(response => {
        if (response.ok) {
          alert('Message sent successfully!');
        } else {
          alert('Failed to send the message.');
        }
      })
      .catch(error => {
        alert('Error: ' + error.message);
      });
  }