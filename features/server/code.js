const url = "http://7957-212-175-155-166.ngrok-free.app/co2";

const apiResponse = await Functions.makeHttpRequest({
    url,
    headers: { 'ngrok-skip-browser-warning': 1 } 
});
const { data } = apiResponse;
const returnvalue = Number(data.emission);
return Functions.encodeUint256(Math.round(data.emission));