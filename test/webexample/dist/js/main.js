(() => {
	if (navigator.mediaDevices && navigator.mediaDevices.getUserMedia) {
		const num = (n) =>
			n >= 10 ? Math.floor(n) : '0' + Math.floor(n);

		const audiomime = 'audio/ogg; codecs=opus';

		const handler = (strm, autorun = false) => {
			const mediaRec = new MediaRecorder(strm, {mimeType: audiomime});
			const contentEl = document.getElementById('content');
			const btnEl = document.getElementById('run');
			const abtEl = document.getElementById('abort');
			const currmdlEl = document.getElementById('currmdlsel');
			const ldmdlEl = document.getElementById('ldmdls');
			mediaRec.aborted = false;
			let timer = null;

			abtEl.onclick = _ => {
				if (timer != null) {
					clearInterval(timer);
					timer = null;
				}

				btnEl.disabled = '';
				btnEl.value = 'Start recording';
				abtEl.style.display = null;
				mediaRec.aborted = true;
				mediaRec.stop();
			};

			btnEl.onclick = _ => {
				if (mediaRec.state === 'recording') {
					if (timer != null) {
						clearInterval(timer);
						timer = null;
					}

					abtEl.style.display = null;
					btnEl.value = 'Transcribing...';
					btnEl.disabled = 'disabled';
					mediaRec.stop();
				} else if (mediaRec.state === 'inactive') {
					try {
						mediaRec.start();
					} catch(err) {
						if (confirm('Failed to run recording, try again?')) {
							run(true);
						}
						return;
					}

					abtEl.style.display = 'block';
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
				if (mediaRec.aborted === true) {
					mediaRec.aborted = false;
					return;
				}

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

								contentEl.innerHTML += `<div class="row"><div class="row-text">${res.recognized}</div><div class="row-btns"><input type="button" value="|>"/><input type="button" value="X"/><audio src="${URL.createObjectURL(new Blob([arraybuf], {type: audiomime}))}"></div></div>`;
								console.log(`Recognition time: ${res.time} seconds`);
								return;
							}

							alert(`HTTP error: ${xhr.status}`);
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
				if (xhr.readyState === XMLHttpRequest.DONE) {
					const res = xhr.response;
					if (xhr.status === 200 && res && typeof res === 'object') {
						const els = [], oels = [];
						for (let i = 0; i < res.length; ++i) {
							const info = res[i];
							if (i % 4 === 3) els.push('<br>');
							els.push(
								`<div><input type="checkbox" id="cbm_${i}" data-id="${i}" ${info[1] ? 'checked' : ''}> <label for="cbm_${i}">${info[0]}</label></div>`
							);
							if (info[1]) oels.push(`<option data-id="${i}">${info[0]}</option>`);
						}
						ldmdlEl.innerHTML = els.join('');
						currmdlEl.innerHTML = oels.join('');
						btnEl.disabled = currmdlEl.selectedIndex < 0 ? 'disabled' : '';
						return;
					}

					alert(`HTTP error: ${xhr.status}`);
				}
			};
			xhr.open('GET', '/models');
			xhr.send();

			contentEl.addEventListener('click', ({target}) => {
				if (target.tagName === 'INPUT') {
					const audio = target.parentNode.querySelector('audio');

					switch (target.value) {
						case '|>':
							if (audio.onpause === null)
								audio.onpause = () => target.value = '|>';
							audio.volume = 0.5;
							audio.play();
							target.value = '||';
							break;
						case '||':
							target.value = '|>';
							audio.pause();
							break;
						case 'X':
							target.parentNode.parentNode.remove();
							break;
					}
				}
			}, true);

			ldmdlEl.addEventListener('click', ({target}) => {
				if (target.tagName === 'INPUT') {
					const boxes = ldmdlEl.querySelectorAll('input[type="checkbox"]')
					for (let i = 0; i < boxes.length; ++i) boxes[i].disabled = 'disabled';
					btnEl.disabled = 'disabled';

					const xhre = new XMLHttpRequest();
					xhre.responseType = 'json';
					xhre.onreadystatechange = _ => {
						if (xhre.readyState === XMLHttpRequest.DONE) {
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
				if (confirm((`Error occured in getUserMedia call: ${err}\r\n\r\nTry again?`))) run();
			});
		};

		run();
	} else {
		alert('No getUserMedia support is present in your browser');
	}
})();
