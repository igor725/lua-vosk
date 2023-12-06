(() => {
	if (navigator.mediaDevices && navigator.mediaDevices.getUserMedia) {
		const num = (n) =>
			n >= 10 ? Math.floor(n) : '0' + Math.floor(n);

		const handler = (strm, autorun = false) => {
			const mediaRec = new MediaRecorder(strm, {mimeType: 'audio/ogg; codecs=opus'});
			const textEl = document.getElementById('text');
			const btnEl = document.getElementById('run');
			const currmdlEl = document.getElementById('currmdlsel');
			const ldmdlEl = document.getElementById('ldmdls');
			let timer = null;

			btnEl.onclick = _ => {
				if (mediaRec.state === 'recording') {
					if (timer != null) {
						clearInterval(timer);
						timer = null;
					}

					btnEl.value = 'Transcribing...';
					btnEl.disabled = 'disabled';
					mediaRec.stop();
				} else if (mediaRec.state === 'inactive') {
					try {
						mediaRec.start();
					} catch(err) {
						if (confirm('Failed to run recoring, try again?')) {
							run(true);
						}
						return;
					}

					btnEl.value = 'Stop recording (00:00)';
					if (timer === null) {
						let time = 0;
						timer = setInterval(() => {
							btnEl.value = `Stop recording (${num(time / (1000 * 60))}:${num((time / 1000) % 60)})`;
							time += 250;
						}, 250);
					}
				}
			};

			mediaRec.ondataavailable = e => {
				e.data.arrayBuffer().then(arraybuf => {
					const xhr = new XMLHttpRequest();
					xhr.responseType = 'json';
					xhr.onreadystatechange = _ => {
						if (xhr.readyState === XMLHttpRequest.DONE) {
							btnEl.disabled = '';
							btnEl.value = 'Start recording';

							const res = xhr.response;
							if (xhr.status === 200 && res && typeof res === 'object') {
								if (res.error !== undefined) {
									alert(res.error);
									return;
								}

								textEl.innerHTML += res.recognized + '\n';
								console.log(`Recognition time: ${res.time} seconds`)
							}
						}
					};
					xhr.open('POST', '/rec');
					xhr.setRequestHeader('Model', currmdlEl.selectedOptions[0].dataset.id);
					xhr.send(arraybuf);
				});
			};

			const xhr = new XMLHttpRequest();
			xhr.responseType = 'json';
			xhr.onreadystatechange = _ => {
				if (xhr.readyState == XMLHttpRequest.DONE) {
					const res = xhr.response;
					if (xhr.status === 200 && res && typeof res === 'object') {
						const els = [], oels = [];
						for (let i = 0; i < res.length; ++i) {
							const info = res[i];
							if (i % 4 == 3) els.push('<br>');
							els.push(
								`<div><input type="checkbox" id="cbm_${i}" data-id="${i}" ${info[1] ? 'checked' : ''}> <label for="cbm_${i}">${info[0]}</label></div>`
							);
							if (info[1]) oels.push(`<option data-id="${i}">${info[0]}</option>`)
						}
						ldmdlEl.innerHTML = els.join('');
						currmdlEl.innerHTML = oels.join('');
						btnEl.disabled = currmdlEl.selectedIndex < 0 ? 'disabled' : '';
					}
				}
			};
			xhr.open('GET', '/models');
			xhr.send();

			ldmdlEl.addEventListener('click', ({target}) => {
				if (target.tagName === 'INPUT') {
					const boxes = ldmdlEl.querySelectorAll('input[type="checkbox"]')
					for (let i = 0; i < boxes.length; ++i) boxes[i].disabled = 'disabled';
					btnEl.disabled = 'disabled';

					const xhre = new XMLHttpRequest();
					xhre.responseType = 'json';
					xhre.onreadystatechange = _ => {
						if (xhre.readyState == XMLHttpRequest.DONE) {
							const res = xhre.response;
							if ((!res || typeof res != 'object') || !res.ok) {
								target.checked = !target.checked;
								alert(res ? res.error : 'Unexpected error');
							}

							const els = [];
							for (let i = 0; i < boxes.length; ++i) {
								const cb = boxes[i];
								cb.disabled = '';
								if (cb.checked) {
									const name = cb.nextElementSibling.innerText;
									els.push(
										`<option data-id="${cb.dataset.id}">${name}</option>`
									);
								}
							}
							currmdlEl.innerHTML = els.join('');
							btnEl.disabled = currmdlEl.selectedIndex < 0 ? 'disabled' : '';
						}
					};
					xhre.open('PUT', target.checked ? '/on' : '/off');
					xhre.setRequestHeader('Model', target.dataset.id);
					xhre.send();
				}
			}, true);

			if (autorun) btnEl.click();
		};

		const run = (autorun = false) => {
			navigator.mediaDevices.getUserMedia({audio: {channelCount: 1}}).then(strm => handler(strm, autorun)).catch((err) => {
				alert(`Error occured in getUserMedia call: ${err}`);
			});
		};

		run();
	} else {
		alert('No getUserMedia support is present in your browser');
	}
})();
