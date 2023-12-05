(() => {
	if (navigator.mediaDevices && navigator.mediaDevices.getUserMedia) {
		navigator.mediaDevices.getUserMedia({audio: {channelCount: 1}}).then((strm) => {
			const mediaRec = new MediaRecorder(strm, {mimeType: 'audio/ogg; codecs=opus'});
			const textEl = document.getElementById('text');
			const btnEl = document.getElementById('run');
			const modelEl = document.getElementById('model');

			btnEl.onclick = _ => {
				if (mediaRec.state === 'recording') {
					btnEl.value = 'Transcribing...';
					btnEl.disabled = 'disabled';
					mediaRec.stop();
				} else if (mediaRec.state === 'inactive') {
					btnEl.value = 'Stop recording';
					mediaRec.start();
				}
			};

			mediaRec.ondataavailable = e => {
				e.data.arrayBuffer().then(arraybuf => {
					const xhr = new XMLHttpRequest();
					xhr.responseType = 'json';
					xhr.onreadystatechange = _ => {
						if (xhr.readyState === XMLHttpRequest.DONE) {
							const res = xhr.response;
							if (xhr.status === 200 && res && typeof res === 'object') {
								if (res.error !== undefined) {
									alert(res.error);
									return;
								}

								btnEl.disabled = '';
								btnEl.value = 'Start recording';
								textEl.innerHTML += res.recognized + '\n';
								modelEl.innerText = res.model;
								console.log(`Recognition time: ${res.time} seconds`)
							}
						}
					};
					xhr.open('POST', '/rec');
					xhr.send(new Uint8Array(arraybuf));
				});
			};

			const xhr = new XMLHttpRequest();
			xhr.onreadystatechange = _ => {
				if (xhr.readyState == XMLHttpRequest.DONE) {
					if (xhr.status === 200)
						modelEl.innerText = xhr.responseText;
				}
			};
			xhr.open('GET', '/model');
			xhr.send();
		}).catch((err) => {
			alert(`Error occured in getUserMedia call: ${err}`);
		});
	} else {
		alert('No getUserMedia support is present in your browser');
	}
})();
