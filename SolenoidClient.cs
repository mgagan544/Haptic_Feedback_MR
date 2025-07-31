using UnityEngine;
using System.Net.Http;
using System.Threading.Tasks;

public class SolenoidClient : MonoBehaviour
{
    [Tooltip("Base URL of the ESP8266, e.g., http://192.168.1.50/")]
    public string baseUrl = "http://192.168.1.50/";

    [Header("Endpoint Options")]
    [Tooltip("Use new single-character endpoints (/start, /stop) for better reliability")]
    public bool useSingleCharEndpoints = true;

    private static readonly HttpClient httpClient = new HttpClient();

    void Start()
    {
        // Set a reasonable timeout for HTTP requests
        httpClient.Timeout = System.TimeSpan.FromSeconds(5);
    }

    // Start the solenoid pulse
    public async void StartPulse()
    {
        string endpoint = useSingleCharEndpoints ? "/start" : "/enable";
        await Send(endpoint);
    }

    // Stop the solenoid pulse
    public async void StopPulse()
    {
        string endpoint = useSingleCharEndpoints ? "/stop" : "/disable";
        await Send(endpoint);
    }

    // Optional: Set BPM
    public async void SetBpm(int bpm) => await Send($"/bpm?val={bpm}");

    private async Task Send(string path)
    {
        try
        {
            string fullUrl = baseUrl.TrimEnd('/') + path;
            Debug.Log($"[SolenoidClient] Sending request to: {fullUrl}");

            string response = await httpClient.GetStringAsync(fullUrl);
            Debug.Log($"[SolenoidClient] {path} -> {response}");
        }
        catch (HttpRequestException ex)
        {
            Debug.LogError($"[SolenoidClient] {path} error: {ex.Message}");
        }
        catch (System.Threading.Tasks.TaskCanceledException ex)
        {
            Debug.LogError($"[SolenoidClient] {path} timeout: {ex.Message}");
        }
    }

    // Manual test methods (right-click component in Inspector)
    [ContextMenu("Test Start Pulse")]
    void TestStartPulse()
    {
        StartPulse();
    }

    [ContextMenu("Test Stop Pulse")]
    void TestStopPulse()
    {
        StopPulse();
    }

    [ContextMenu("Test Set BPM 80")]
    void TestSetBPM()
    {
        SetBpm(80);
    }
}