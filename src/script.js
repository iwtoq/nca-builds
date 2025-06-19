const platformTabs = document.getElementsByClassName("platform-tabs")[0];
const versionsPanel = document.getElementById("versions-panel");
var platforms = {};
var currentPlatform = "";
getBuilds();

async function getBuilds() {
	const response = await fetch("builds/builds.json");
	if (!response.ok) {
		throw new Error(`Response status: ${response.status}`);
	}

	platforms = await response.json();

	const DEFAULT_SELECT = "Windows";
	appendPlatformButtons(DEFAULT_SELECT);

	document.getElementById("hidden").style.display = "block";
}

function selectPlatform(newPlatformName) {
	return function (e) {
		deletePlatformButtons();
		deleteVersions();
		appendPlatformButtons(newPlatformName);
	};
}

function appendPlatformButtons(selectedPlatformName) {
	for (const platform in platforms) {
		const button = document.createElement("button");
		button.type = "button";
		button.textContent = platform;
		button.addEventListener("click", selectPlatform(platform));

		if (platform == selectedPlatformName)
			button.className = "platform-button-on";
		else button.className = "platform-button-off";

		platformTabs.append(button);
	}
	currentPlatform = selectedPlatformName;
	appendVersions();
}

function deletePlatformButtons() {
	const platformButtons = Array.from(platformTabs.children);

	platformButtons.forEach((button) => {
		platformTabs.removeChild(button);
	});
}

document.getElementById("search").addEventListener("input", function () {
	showVersions(this.value);
});

function showVersions(searchQuery)
{
	deleteVersions();
	appendVersions(searchQuery);
}

function appendVersions(searchQuery) {
	const versions = platforms[currentPlatform];

	let shownVersions = 0;
	versions.forEach((version) => {
		if (!searchQuery || version.includes(searchQuery)) {
			shownVersions++;

			const linkElement = document.createElement("a");
			const ext = currentPlatform == "Android" ? "apk" : "zip"
			linkElement.href = `builds/${currentPlatform}/${version}/Nameless Cat Architect.${ext}`;
			linkElement.textContent = version;

			versionsPanel.appendChild(linkElement);
			versionsPanel.appendChild(document.createElement("br"));
		}
	});
	if (shownVersions > 0)
		versionsPanel.style.display = "block";
}

function deleteVersions() {
	const versionLinks = Array.from(versionsPanel.children);

	versionLinks.forEach((version) => {
		versionsPanel.removeChild(version);
	});
	versionsPanel.style.display = "none";
}