using UnityEngine;
using System.Collections.Generic;

[RequireComponent(typeof(Collider))]
public class SolenoidTrigger : MonoBehaviour
{
    [Header("Solenoid Client Reference")]
    public SolenoidClient solenoid;

    [Header("Hand/Controller Tags")]
    public string leftControllerTag = "newtag1L";
    public string rightControllerTag = "newtag1R";

    [Header("Optional Extra Tag (e.g., Player Root)")]
    public string extraTag = "";  // leave blank if unused

    [Header("Debug Settings")]
    public bool enableDetailedLogging = true;

    // Track which specific colliders are inside
    private readonly HashSet<Collider> _inside = new HashSet<Collider>();

    // Public read-only state
    [SerializeField] private bool isControllerInside = false;
    public bool IsControllerInside => isControllerInside;

    void Awake()
    {
        Collider col = GetComponent<Collider>();
        if (col)
        {
            col.isTrigger = true;
        }
        else
        {
            Debug.LogError("[SolenoidTrigger] No Collider found on " + gameObject.name);
        }

        if (solenoid == null)
            Debug.LogWarning("[SolenoidTrigger] SolenoidClient not assigned!");
    }

    bool IsRelevant(Collider other)
    {
        if (other.CompareTag(leftControllerTag)) return true;
        if (other.CompareTag(rightControllerTag)) return true;
        if (!string.IsNullOrEmpty(extraTag) && other.CompareTag(extraTag)) return true;
        return false;
    }

    void OnTriggerEnter(Collider other)
    {
        if (enableDetailedLogging)
            Debug.Log($"[SolenoidTrigger] ENTER from '{other.name}' tag='{other.tag}'");

        if (!IsRelevant(other)) return;

        bool wasEmpty = _inside.Count == 0;
        _inside.Add(other);

        if (enableDetailedLogging)
            Debug.Log($"[SolenoidTrigger] Relevant collider added. Count: {_inside.Count}");

        if (wasEmpty && !isControllerInside)
        {
            isControllerInside = true;
            Debug.Log("[SolenoidTrigger] First relevant collider entered -> START Pulse.");
            solenoid?.StartPulse(); // Updated method name
        }
    }

    void OnTriggerExit(Collider other)
    {
        if (enableDetailedLogging)
            Debug.Log($"[SolenoidTrigger] EXIT from '{other.name}' tag='{other.tag}'");

        if (!IsRelevant(other)) return;

        if (_inside.Remove(other))
        {
            if (enableDetailedLogging)
                Debug.Log($"[SolenoidTrigger] Relevant collider removed. Count: {_inside.Count}");

            if (_inside.Count == 0 && isControllerInside)
            {
                isControllerInside = false;
                Debug.Log("[SolenoidTrigger] Last relevant collider exited -> STOP Pulse.");
                solenoid?.StopPulse(); // Updated method name
            }
        }
    }

    void Update()
    {
        // Clean up any null references (destroyed objects)
        _inside.RemoveWhere(collider => collider == null);

        // Safety fallback: if no controllers inside but still active
        if (_inside.Count == 0 && isControllerInside)
        {
            Debug.LogWarning("[SolenoidTrigger] Forced Stop (fallback) - no controllers detected");
            solenoid?.StopPulse();
            isControllerInside = false;
        }
    }

    // Safety net: if something destroys the colliders but never fires Exit
    void OnDisable()
    {
        if (isControllerInside)
        {
            Debug.LogWarning("[SolenoidTrigger] Trigger disabled while active -> forcing STOP.");
            solenoid?.StopPulse(); // Updated method name
            isControllerInside = false;
            _inside.Clear();
        }
    }

    void OnDestroy()
    {
        // Clean up when destroyed
        if (isControllerInside)
        {
            Debug.Log("[SolenoidTrigger] OnDestroy -> STOP Pulse.");
            solenoid?.StopPulse(); // Updated method name
        }
    }

    // Debug methods for testing (right-click component in Inspector)
    [ContextMenu("Debug Status")]
    void DebugStatus()
    {
        Debug.Log($"[SolenoidTrigger] Status - Active: {isControllerInside}, Controllers Inside: {_inside.Count}");
        foreach (var collider in _inside)
        {
            if (collider != null)
                Debug.Log($"  - {collider.name} ({collider.tag})");
        }
    }

    [ContextMenu("Force Stop")]
    void ForceStop()
    {
        Debug.Log("[SolenoidTrigger] Manual Stop");
        solenoid?.StopPulse();
        isControllerInside = false;
        _inside.Clear();
    }

    [ContextMenu("Force Start")]
    void ForceStart()
    {
        Debug.Log("[SolenoidTrigger] Manual Start");
        solenoid?.StartPulse();
        isControllerInside = true;
    }
}