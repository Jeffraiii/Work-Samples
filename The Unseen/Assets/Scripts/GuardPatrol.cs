using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GuardPatrol : MonoBehaviour {
    //public float waitTime;
    public Transform[] points;
    private int destPoint = 0;
    private UnityEngine.AI.NavMeshAgent agent;

    // Use this for initialization
    void Start () {
        agent = GetComponent<UnityEngine.AI.NavMeshAgent>();
        
        // Disabling auto-braking allows for continuous movement
        // between points (ie, the agent doesn't slow down as it
        // approaches a destination point).
        agent.autoBraking = false;

        GotoNextPoint();
    }

    // Update is called once per frame
    void Update()
    {
        // Choose the next destination point when the agent gets
        // close to the current one.
        if (!agent.pathPending && agent.remainingDistance < 0.25f) {
            GuardStopAndLook();
            GotoNextPoint();
        }
    }
    void GotoNextPoint()
    {
        // Returns if no points have been set up
        if (points.Length == 0)
            return;


        // Set the agent to go to the currently selected destination.
        agent.destination = points[destPoint].position;

        // Choose the next point in the array as the destination,
        // cycling to the start if necessary.
        destPoint = (destPoint + 1) % points.Length;
    }

   private void GuardStopAndLook()
    {
        /*agent.isStopped = true;
        if (waitTime >= 0f)
        {
            waitTime -= Time.deltaTime;
        }
        agent.isStopped = false;*/
    }

}
