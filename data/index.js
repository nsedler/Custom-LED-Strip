let address = "http://192.168.4.48/api/led";
let power = true;

function handlePowerState() {
    fetch(`${address}/power`, {
        method: "POST"
    }).then(() => console.log(`power set to ${!power}`)).catch(e => console.log(e));
    power = !power;
}

function handleModeChange(newMode) {
    fetch(`${address}/mode?mode=${newMode}`, {
        method: "POST"
    }).then(() => console.log(`${newMode} Mode Activated`)).catch(e => console.log(e));
}

function handleColor(newColor) {
    fetch(`${address}/color?color=${newColor}`, {
        method: "POST"
    }).then(() => console.log(`Color set to ${newColor}`)).catch(e => console.log(e));
}

function handleCustomColor(customColor) {
    customColor = hexToRgb(customColor);
    fetch(`${address}/color?r=${customColor.r}&g=${customColor.g}&b=${customColor.b}`, {
        method: "POST"
    }).then(() => console.log(`Color set to ${customColor.r},${customColor.g},${customColor.b}`)).catch(e => console.log(e));
}

function hexToRgb(hex) {
    hex = hex.replace(/^#/, '');
    const bigint = parseInt(hex, 16);
    const r = (bigint >> 16) & 255;
    const g = (bigint >> 8) & 255;
    const b = bigint & 255;
    return {
        r,
        g,
        b
    };
}

function handleSliderChange(elementName) {
    var slider = document.getElementById(elementName);
    var sliderValue = slider.value;
    if (elementName == 'brightnessSlider') {
        fetch(`${address}/brightness?brightness=${sliderValue}`, {
            method: "POST"
        }).then(() => console.log(`Brightness set to ${sliderValue}`)).catch(e => console.log(e));
    } else if (elementName == "intervalSlider") {
        fetch(`${address}/interval?interval=${sliderValue}`, {
            method: "POST"
        }).then(() => console.log(`Interval set to ${sliderValue}`)).catch(e => console.log(e));
    }
}
var colorPicker = new iro.ColorPicker("#picker", {
    // Set the size of the color picker
    width: 150,
    // Set the initial color to pure red
    color: "#00FF00",
    wheelLightness: false
});

colorPicker.on('input:end', function (color) {
    // log the current color as a HEX string
    customColor = hexToRgb(color.hexString);
    fetch(`${address}/color?r=${customColor.r}&g=${customColor.g}&b=${customColor.b}`, {
        method: "POST"
    }).then(() => console.log(`Color set to ${customColor.r},${customColor.g},${customColor.b}`)).catch(e => console.log(e));
});