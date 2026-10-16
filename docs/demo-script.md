# Hackathon demo script (3–4 minutes)

1. Open the lab and point out that scenario fixtures come from the live API.
2. Select the two-process deadlock. Explain the two allocation edges and two outstanding requests.
3. Run detection. Step through Work, component comparisons, and the unfinished set P1/P2.
4. Select P1 in Recovery and confirm termination. Show its allocation row cleared and R1 returned.
5. Explain that the API reran detection and P2 can now finish.
6. Switch to multi-instance fixture. Explain why a RAG cycle alone cannot establish deadlock when resource types have multiple instances.
7. Finish with the Guide’s four Coffman conditions and state the model limitations honestly.
