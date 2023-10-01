let address = "http://192.168.4.48/api/led";
let power = true;
function handlePowerState() {
    fetch(`${address}/power`, { method: "POST" })
        .then(() => console.log(`power set to ${!power}`))
        .catch(e => console.log(e)); power = !power;
}
function handleModeChange(newMode) {
    fetch(`${address}/mode?mode=${newMode}`, { method: "POST" })
        .then(() => console.log(`${newMode} Mode Activated`))
        .catch(e => console.log(e));
}
function handleSliderChange(elementName) {
    var slider = document.getElementById(elementName);
    var sliderValue = slider.value;
    if (elementName == 'brightnessSlider') {
        fetch(`${address}/brightness?brightness=${sliderValue}`, { method: "POST" })
            .then(() => console.log(`Brightness set to ${sliderValue}`))
            .catch(e => console.log(e));
    } else if (elementName == "intervalSlider") {
        fetch(`${address}/interval?interval=${sliderValue}`, { method: "POST" })
            .then(() => console.log(`Interval set to ${sliderValue}`))
            .catch(e => console.log(e));
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
    var hsv = color.hsv;
    fetch(`${address}/color?hue=${hsv.h}&saturation=${hsv.s}&value=${hsv.v}`, { method: "POST" })
        .then(() => console.log(`Color set to HSV(${hsv.h},${hsv.s},${hsv.v})`))
        .catch(e => console.log(e));
});